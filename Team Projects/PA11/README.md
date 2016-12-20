Programming Assignment 11: Labyrinth
=========================
Team Members
------------
Marco Rivas, Martin Revilla, Truman Chan

Description
-----------
The purpose of this assignment is to produce a 3D simulation of the Labyrinth game that is as realistic to the actual game as possible, yet still being a playable and an enjoyable experience for the end user.

Controls
--------
Mouse Control             | Action
------------------------- | ------
Hold left button and move | Tilt table
Right-Click               | Open menu
Scroll Wheel Up           | Zoom camera in
Scroll Wheel Down         | Zoom camera out

Keyboard Input | Action
-------------------- | ------
'x'                  | Play/Pause song
Spacebar             | Play next song
Up Arrow Key         | Move camera up
Left Arrow Key       | Move camera left
Down Arrow Key       | Move camera down
Right Arrow Key      | Move camera right
Esc                  | Exit game

PS4 Controller Input | Action
-------------------- | ------
Left Analog Stick | Tilt table

Lighting
--------
- Ambient
- Ball Spotlight
- Goal Spotlights

Extra Credit
------------
- Game audio/music
- PS4 Controllers
- Top 10 Scoreboard
- Multiple levels

Installations
---------------------
Install GLUT, GLEW, and GLM:
>$ sudo apt-get install freeglut3-dev freeglut3 libglew1.6-dev libglm-dev

Install Assimp for model loading:
>$ sudo apt-get install libassimp-dev

Install SFML gaming library for game audio. To install SFML:
>$ sudo apt-get install libsfml-dev

Install Magick++ to load its images. To install Magick++:
>$ sudo apt-get install libmagick++-dev

Install Bullet for the physics engine. To install Bullet:
>$ sudo apt-get install libbullet-dev

Building This Assignment
---------------------
To build this assignment:
>$ cd build

>$ make

To clean (remove object files and executables):
>$ cd build

>$ make clean

Running This Assignment
---------------------
The executable will be put in the bin directory. To run Labyrinth:
>$ cd bin

>$ ./Labyrinth



