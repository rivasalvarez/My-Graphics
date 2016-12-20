# Directories
BIN_DIR = ../bin
BUILD_DIR = ../build
SRC_DIR = ../src

# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/shaderClass.o $(BUILD_DIR)/modelLoader.o

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x
   
all: $(OBJ) $(BIN_DIR)/CS480

$(BIN_DIR)/CS480: $(OBJ)
	$(CC) $(CXXFLAGS) $(OBJ) -o $(BIN_DIR)/CS480 $(LIBS)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/shaderClass.h
	$(CC) $(CXXFLAGS) -o $(BUILD_DIR)/main.o -c $(SRC_DIR)/main.cpp
	
$(BUILD_DIR)/shaderClass.o: $(SRC_DIR)/shaderClass.cpp $(SRC_DIR)/shaderClass.h $(SRC_DIR)/modelLoader.h
	$(CC) $(CXXFLAGS) -o $(BUILD_DIR)/shaderClass.o -c $(SRC_DIR)/shaderClass.cpp

$(BUILD_DIR)/modelLoader.o: $(SRC_DIR)/modelLoader.cpp $(SRC_DIR)/modelLoader.h
	$(CC) $(CXXFLAGS) -o $(BUILD_DIR)/modelLoader.o -c $(SRC_DIR)/modelLoader.cpp
	
clean:
	rm -rf $(BIN_DIR)/CS480 $(OBJ)

