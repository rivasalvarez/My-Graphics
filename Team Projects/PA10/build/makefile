# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp `Magick++-config --cppflags --cxxflags --ldflags --libs` -I/usr/include/bullet -lBulletDynamics -lBulletSoftBody -lBulletCollision -lLinearMath

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/CS480

../bin/CS480: ../src/main.cpp
	$(CC) $(CXXFLAGS) ../src/main.cpp -o ../bin/CS480 $(LIBS)

clean:
	rm *.o ../bin/CS480
