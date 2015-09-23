# Directories
BIN_DIR = ../bin
BUILD_DIR = ../build
SRC_DIR = ../src

# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/Shader.o

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x
   
all: $(OBJ) $(BIN_DIR)/Matrix

$(BIN_DIR)/Matrix: $(OBJ)
	$(CC) $(CXXFLAGS) $(OBJ) -o $(BIN_DIR)/Matrix $(LIBS)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/Shader.h
	$(CC) $(CXXFLAGS) -o $(BUILD_DIR)/main.o -c $(SRC_DIR)/main.cpp
	
$(BUILD_DIR)/Shader.o: $(SRC_DIR)/Shader.cpp $(SRC_DIR)/Shader.h
	$(CC) $(CXXFLAGS) -o $(BUILD_DIR)/Shader.o -c $(SRC_DIR)/Shader.cpp

clean:
	rm -rf $(BIN_DIR)/Matrix $(OBJ)
