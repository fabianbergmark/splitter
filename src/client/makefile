CC=gcc
CXX=g++
CFLAGS=
CXXFLAGS=-O0 -g -std=c++0x -I../lib
APP=splitter-client
BINDIR=../../bin/client
BIN=$(BINDIR)/$(APP)

.DEFAULT: all
.PHONY: all clean again

SRC=client.cpp \
    config.cpp
OBJ=$(SRC:.cpp=.o)
LIBS=-lsctp
SPLITTERLIB=../lib/splitter/splitter.a

all: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) $(SPLITTERLIB) -o$(BIN)

clean:
	rm -f $(OBJ)
	rm -f $(BIN)

again: clean .DEFAULT
