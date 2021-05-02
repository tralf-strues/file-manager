# Options = -Wall -Wpedantic

# SrcDir = src
# BinDir = bin

# $(BinDir)/file_manager.a: $(BinDir)/file_manager.o
# 	ar rcs $(BinDir)/file_manager.a $(BinDir)/file_manager.o
	
# $(BinDir)/file_manager.o: $(SrcDir)/file_manager.cpp $(SrcDir)/file_manager.h
# 	g++ -c $(SrcDir)/file_manager.cpp -o $(BinDir)/file_manager.o $(Options)

# -----------------------------------Constants----------------------------------
AllWarnings    = -Wall -Wextra -pedantic
SomeWarnings   = -Wall -Wextra
LittleWarnings = -Wall
NoWarnings     = 
# -----------------------------------Constants----------------------------------

# ------------------------------------Options-----------------------------------
PREFIX   = /usr
LXXFLAGS = 
CXXFLAGS = -std=c++20 -O2 $(AllWarnings)
# ------------------------------------Options-----------------------------------

# -------------------------------------Files------------------------------------
SrcDir = src
BinDir = bin
IntDir = $(BinDir)/intermediates
LibDir = file_manager

Deps   = $(wildcard $(SrcDir)/*.h)
CppSrc = $(notdir $(wildcard $(SrcDir)/*.cpp)) 

Objs    = $(addprefix $(IntDir)/, $(CppSrc:.cpp=.o))
Archive = file_manager.a
# -------------------------------------Files------------------------------------

# ----------------------------------Make rules----------------------------------
.PHONY: build install install-bin uninstall clean

build: $(BinDir)/$(Archive)
$(BinDir)/$(Archive): $(Objs) $(Deps)
	ar ru $@ $(IntDir)/file_manager.o

$(IntDir)/file_manager.o: $(SrcDir)/file_manager.cpp $(SrcDir)/file_manager.h
	$(CXX) -o $(IntDir)/file_manager.o -c $(SrcDir)/file_manager.cpp

install: install-bin build
	mkdir -p $(DESTDIR)$(PREFIX)/lib/$(LibDir)
	mkdir -p $(DESTDIR)$(PREFIX)/include/$(LibDir)
	cp $(BinDir)/$(Archive) $(DESTDIR)$(PREFIX)/lib/$(LibDir)
	cp $(SrcDir)/file_manager.h $(DESTDIR)$(PREFIX)/include/$(LibDir)

install-bin:
	mkdir -p $(IntDir)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/$(LibDir)
	rm -f $(DESTDIR)$(PREFIX)/include/$(LibDir)

clean:
	rm -f $(Objs) $(BinDir)/$(Archive)
# ----------------------------------Make rules----------------------------------