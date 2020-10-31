Options = -Wall -Wpedantic

SrcDir = src
BinDir = bin

$(BinDir)\file_manager.a: $(BinDir)\file_manager.o
	ar ru $(BinDir)\file_manager.a $(BinDir)\file_manager.o
	
$(BinDir)\file_manager.o: $(SrcDir)\file_manager.cpp $(SrcDir)\file_manager.h
	g++ -c $(SrcDir)\file_manager.cpp -o $(BinDir)\file_manager.o $(Options)