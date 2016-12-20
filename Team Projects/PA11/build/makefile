# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp `Magick++-config --cppflags --cxxflags --ldflags --libs` -I/usr/include/bullet -lBulletDynamics -lBulletSoftBody -lBulletCollision -lLinearMath -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system

SFML= -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system
# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/Labyrinth

../bin/main.o: ../src/main.cpp
	$(CC) $(CXXFLAGS) ../src/main.cpp -c $(LIBS)

../bin/GameEngine.o: ../src/GameEngine.cpp
	$(CC) $(CXXFLAGS) ../src/GameEngine.cpp -c $(LIBS)

../bin/MusicPlayer.o: ../src/MusicPlayer.cpp
	$(CC) $(CXXFLAGS) ../src/MusicPlayer.cpp -c $(LIBS)

../bin/Graphics.o: ../src/Graphics.cpp
	$(CC) $(CXXFLAGS) ../src/Graphics.cpp -c $(LIBS)

../bin/PhysicsEngine.o: ../src/PhysicsEngine.cpp
	$(CC) $(CXXFLAGS) ../src/PhysicsEngine.cpp -c $(LIBS)

../bin/GameController.o: ../src/GameController.cpp
	$(CC) $(CXXFLAGS) ../src/GameController.cpp -c $(LIBS)

../bin/Labyrinth: ../bin/main.o ../bin/MusicPlayer.o ../bin/GameController.o ../bin/GameEngine.o ../bin/PhysicsEngine.o ../bin/Graphics.o
	$(CC) $(CXXFLAGS) -o ../bin/Labyrinth main.o MusicPlayer.o GameController.o GameEngine.o PhysicsEngine.o Graphics.o $(LIBS)

clean:
	rm *.o ../bin/Labyrinth
	

