# Compiler and Compile options.
CC = g++
CXXFLAGS = -g -Wall -O3 -std=c++11
MAKEFLAGS += -j

# Macros specifying path for compile.
SRCS := $(wildcard src/*.cc) 
OBJS := $(SRCS:.cpp=.o)
BIN = ./bin/
INC = ./include/
LIB = ./lib/ -lpthread

# Pre-Processor.
CPPFLAGS += -I$(INC)

# Compile command.
TARGET = decisiontree # decision tree
$(TARGET):$(OBJS)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)$(TARGET) $(SRCS) -L$(LIB)

# Delete binary & object files.
clean:
	rm ./bin/*
	rm ./cscope*
