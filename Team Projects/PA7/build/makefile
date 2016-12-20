# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp `Magick++-config --cppflags --cxxflags --ldflags --libs` 

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/SolarSystem

../bin/main.o: ../src/main.cpp
	$(CC) $(CXXFLAGS) ../src/main.cpp -c $(LIBS)

../bin/modelLoader.o: ../src/modelLoader.cpp
	$(CC) $(CXXFLAGS) ../src/modelLoader.cpp -c $(LIBS)

../bin/CelestialBody.o: ../src/CelestialBody.cpp
	$(CC) $(CXXFLAGS) ../src/CelestialBody.cpp -c $(LIBS)

../bin/SolarSystem: ../bin/main.o ../bin/CelestialBody.o ../bin/modelLoader.o
	$(CC) $(CXXFLAGS) -o ../bin/SolarSystem main.o CelestialBody.o modelLoader.o $(LIBS)

clean:
	rm *.o ../bin/SolarSystem
	

