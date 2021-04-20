#include <assert.h>
#include <stdlib.h>
#include <string.h>     
#include <sys/stat.h>  

#include "file_manager.h"

const char* TEXT_CASE_FORMAT_UPPER = "u";
const char* TEXT_CASE_FORMAT_LOWER = "l";

struct Text
{
    size_t bytesCount = 0;
    char*  buffer     = NULL;
    char** lines      = NULL;
    size_t currPos    = 0;
    size_t linesCount = 0;
};

//-----------------------------------------------------------------------------
//! Returns size in bytes of a file.
//!
//! @param [in] filename  name of the file to get size of
//!
//! @return size of a file in bytes or 0 if an error occured.
//-----------------------------------------------------------------------------
size_t getFileSize(const char* fileName)
{
    assert(fileName != NULL);

    struct stat fileStat = {};
    if (stat((char*) fileName, &fileStat) == -1) { return 0; }

    return (size_t) fileStat.st_size;
}

bool loadFile(const char* filename, char** buffer, size_t* bufferSize)
{
    assert(filename   != nullptr);
    assert(buffer     != nullptr);
    assert(bufferSize != nullptr);

    FILE* file = fopen(filename, "r");
    if (file == nullptr) { return false; }

    size_t fileSize = getFileSize(filename);

    *buffer = (char*) calloc(fileSize + 1, sizeof(char));
    assert(*buffer != nullptr);

    *bufferSize = fread(*buffer, sizeof(char), fileSize, file);
    if (*bufferSize == 0) 
    {
        fclose(file);
        free(*buffer);

        *buffer = nullptr;

        return false;
    }

    *(*buffer + *bufferSize) = '\0';

    fclose(file);

    return true;
}

//-----------------------------------------------------------------------------
//! Makes a complete copy of text. 
//!
//! @param [in] text text to be copied
//!
//! @return a pointer to the copy of text made or NULL if an error occurred.
//-----------------------------------------------------------------------------
Text* copyText(Text* text)
{
    assert(text != NULL);

    Text* textCopy = (Text*) calloc(1, sizeof(Text));
    if (textCopy == NULL) { return NULL; }

    textCopy->bytesCount = text->bytesCount;
    textCopy->buffer     = (char*) calloc(textCopy->bytesCount + 1, sizeof(char));
    memcpy(textCopy->buffer, text->buffer, textCopy->bytesCount);

    textCopy->buffer[textCopy->bytesCount] = '\0';

    textCopy->linesCount = text->linesCount;
    textCopy->lines      = (char**) calloc(textCopy->linesCount, sizeof(char*));
    if (textCopy->lines == NULL) 
    { 
        free(textCopy->buffer);

        return NULL;
    }

    size_t currLinePos = 0;
    textCopy->lines[currLinePos++] = &textCopy->buffer[0];
    for (size_t i = 0; i < textCopy->bytesCount - 1; i++)
    {
        if (textCopy->buffer[i] == '\0') { 
            textCopy->lines[currLinePos++] = &textCopy->buffer[i + 1]; 
        }
    }

    textCopy->currPos = 0;

    return textCopy;
}

//-----------------------------------------------------------------------------
//! Reads Text from file. 
//!
//! @param [in] fileName name of the file from which to read Text
//!
//! @note uses dynamic memory, so after finishing to work with the read text
//!       it's recommended to call deleteText(...) function in order to free
//!       used memory.
//!
//! @return a pointer to the text read or NULL if an error occurred.
//-----------------------------------------------------------------------------
Text* readTextFromFile(const char* fileName)
{
    assert(fileName != NULL);

    FILE* file = fopen(fileName, "r");
    if (file == NULL) { return NULL; }

    Text* text = (Text*) calloc(1, sizeof(Text));
    assert(text != NULL);

    text->bytesCount = getFileSize(fileName);
    text->buffer     = (char*) calloc(text->bytesCount, sizeof(char));
    assert(text->buffer != NULL);

    text->bytesCount = fread(text->buffer, sizeof(char), text->bytesCount, file);
    if (text->bytesCount == 0) 
    {
        fclose(file);
        free(text);

        return NULL;
    }

    char* newTextBuffer = (char*) realloc(text->buffer, sizeof(char) * (text->bytesCount + 1));
    assert(newTextBuffer != NULL);
    newTextBuffer[text->bytesCount] = '\0';
    text->buffer = newTextBuffer;

    text->linesCount = replaceAllOccurrences(text->buffer, text->bytesCount, '\n', '\0') + 1;
    text->lines      = (char**) calloc(text->linesCount, sizeof(char*));
    assert(text->lines != NULL);

    size_t currLinePos = 0;
    text->lines[currLinePos++] = &text->buffer[0];
    for (size_t i = 0; i < text->bytesCount - 1; i++)
    {
        if (text->buffer[i] == '\0') { 
            text->lines[currLinePos++] = &text->buffer[i + 1]; 
        }
    }

    text->currPos = 0;

    fclose(file);

    return text;
}

//-----------------------------------------------------------------------------
//! Writes Text to file. 
//!
//! @param [in] file to which to write Text
//! @param [in] text Text which is to be written to file
//!
//!
//! @return a pointer to the text read or NULL if an error occurred.
//-----------------------------------------------------------------------------
bool writeTextToFile(FILE* file, Text* text)
{
    assert(file != NULL);
    assert(text != NULL);

    replaceAllOccurrences(text->buffer, text->bytesCount, '\0', '\n');

    return fwrite(text->buffer, sizeof(char), text->bytesCount, file) == text->bytesCount;
}

//-----------------------------------------------------------------------------
//! Frees memory used for text. 
//!
//! @param [in] text Text object to be deleted
//!
//-----------------------------------------------------------------------------
void deleteText(Text* text)
{
    assert(text != NULL);

    text->bytesCount = 0;
    text->currPos    = 0;
    text->linesCount = 0;

    if (text->buffer != NULL)
    {
        free(text->buffer);
        text->buffer = NULL;
    }

    if (text->lines != NULL)
    {
        free(text->lines);
        text->lines = NULL;
    }

    free(text);
}

//-----------------------------------------------------------------------------
//! Returns next line of text.
//!
//! @param [in] text 
//!
//! @return pointer to the next line's first symbol or NULL in case end of text
//          is reached or an error occured.
//-----------------------------------------------------------------------------
char* nextTextLine(Text* text)
{
    assert(text         != NULL);
    assert(text->lines  != NULL);
    assert(text->buffer != NULL);

    if (text->currPos >= text->linesCount)
    {
        return NULL;
    }

    return text->lines[text->currPos++];
}

//-----------------------------------------------------------------------------
//! Sets internal current line index to 0, so that next calls of, for instance,
//! nextTextLine will start from the beginning.
//!
//! @param [in] text 
//-----------------------------------------------------------------------------
void resetTextToStart(Text* text)
{
    assert(text != NULL);

    text->currPos = 0;
}

//-----------------------------------------------------------------------------
//! Returns ith (starting from 0) line of text.
//!
//! @param [in] text 
//!
//! @return pointer to the ith line's first symbol or NULL in case an error 
//!         occured.
//-----------------------------------------------------------------------------
char* getLine(Text* text, size_t i)
{
    assert(text != NULL);

    return text->lines[i];
}

//-----------------------------------------------------------------------------
//! @param [in] text  
//! 
//! @return current line's number starting from 0.
//-----------------------------------------------------------------------------
size_t getCurrentLineNumber(Text* text)
{
    assert(text != NULL);

    return text->currPos - 1;
}

//-----------------------------------------------------------------------------
//! Replaces all bytes with value 'target' in buffer with 'replacement'.
//!
//! @param [in] buffer       bytes-array to be examined and altered
//! @param [in] bufferSize   number of bytes in buffer
//! @param [in] target       specifies what bytes in buffer to replace
//! @param [in] replacement  specifies what value target bytes to replace with
//!
//! @note Undefined behavior if buffer is of lesser size than bufferSize.
//!
//! @return number of replaced bytes.
//-----------------------------------------------------------------------------
size_t replaceAllOccurrences(char* buffer, size_t bufferSize, char target, char replacement)
{
    assert(buffer != NULL);

    size_t numOfOccurences = 0;
    for (size_t i = 0; i < bufferSize; i++)
    {
        if (buffer[i] == target)
        {
            buffer[i] = replacement;
            numOfOccurences++;
        }
    }

    return numOfOccurences;
}

//-----------------------------------------------------------------------------
//! Converts all upper-case letters in str to lower-case. 
//!
//! @param [in] str 
//!
//! @note Checks characters until '\0' is incountered, so there will be 
//!       undefined behavior if there's no '\0'.
//!
//! @return pointer to str or NULL if an error occured.
//-----------------------------------------------------------------------------
char* strToLower(char* str)
{
    assert(str != NULL);

    for (; *str != '\0'; str++)
    {
        if (*str >= 'A' && *str <= 'Z')
        {
            *str = 'a' + *str - 'A';
        }
    }

    return str;
}

//-----------------------------------------------------------------------------
//! Converts all lower-case letters in str to upper-case. 
//!
//! @param [in] str 
//!
//! @note Checks characters until '\0' is incountered, so there will be 
//!       undefined behavior if there's no '\0'.
//!
//! @return pointer to str or NULL if an error occured.
//-----------------------------------------------------------------------------
char* strToUpper(char* str)
{
    assert(str != NULL);

    for (; *str != '\0'; str++)
    {
        if (*str >= 'a' && *str <= 'z')
        {
            *str = 'A' + *str - 'a';
        }
    }

    return str;
}

//-----------------------------------------------------------------------------
//! Makes text case-homogeneous depending on the flag.
//! Flags
//!  - "u" makes all letter characters upper-case 
//!  - "l" makes all letter characters lower-case 
//!
//! @param [in] text 
//! @param [in] flag 
//!
//! @return text or NULL if an error occurerd.
//-----------------------------------------------------------------------------
Text* makeTextCaseHomogeneous(Text* text, const char* flag)
{
    assert(text != NULL);
    assert(flag != NULL);

    bool isToUpper = false;
    if (strcmp(flag, TEXT_CASE_FORMAT_UPPER) == 0)
    {
        isToUpper = true;
    }
    else if (strcmp(flag, TEXT_CASE_FORMAT_LOWER) == 0)
    {
        isToUpper = false;
    }
    else
    {
        return NULL;
    }

    size_t oldCurrPos = text->currPos;
    text->currPos = 0;

    char* nextLine = NULL;
    while ((nextLine = nextTextLine(text)) != NULL)
    {
        if (isToUpper)
        {
            strToUpper(nextLine);
        }
        else
        {
            strToLower(nextLine);
        }
    }

    text->currPos = oldCurrPos;

    return text;
}