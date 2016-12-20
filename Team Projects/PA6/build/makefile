# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp `Magick++-config --cppflags --cxxflags --ldflags --libs` -I/usr/include/bullet -lBulletDynamics -lBulletSoftBody -lBulletCollision -lLinearMath
# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/CS480

../bin/main.o: ../src/main.cpp
	$(CC) $(CXXFLAGS) ../src/main.cpp -c $(LIBS)

../bin/modelLoader.o: ../src/modelLoader.cpp
	$(CC) $(CXXFLAGS) ../src/modelLoader.cpp -c $(LIBS)

../bin/shaderClass.o: ../src/shaderClass.cpp
	$(CC) $(CXXFLAGS) ../src/shaderClass.cpp -c $(LIBS)

../bin/CS480: ../bin/main.o ../bin/shaderClass.o ../bin/modelLoader.o
	$(CC) $(CXXFLAGS) -o ../bin/CS480 main.o shaderClass.o modelLoader.o $(LIBS)

clean:
	rm *.o ../bin/CS480
	

