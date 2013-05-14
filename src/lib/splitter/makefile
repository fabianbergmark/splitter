CC=gcc
CXX=g++
CFLAGS=
CXXFLAGS=-O0 -g -std=c++0x
APP=splitter

.DEFAULT: all
.PHONY: all clean again

SRC=tcp_connection.cpp \
    sctp_connection.cpp \
    tcp_server.cpp \
    sctp_server.cpp \
    network.cpp \
    config.cpp
OBJ=$(SRC:.cpp=.o)

all: $(OBJ)
	ld -r $(OBJ) -o $(APP).o
	ar crf $(APP).a $(APP).o
clean:
	rm -f $(OBJ)
	rm -f $(APP).o
	rm -f $(APP).a

again: clean .DEFAULT
