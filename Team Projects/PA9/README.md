Programming Assignment 09: Air Hockey
=========================
Team Members
------------
Marco Rivas, Martin Revilla, Truman Chan

Description
-----------
The purpose of this assignment is to produce a 3D simulation of Air Hockey that is as realistic to the actual game as possible, yet still being a playable and an enjoyable experience for the end user. 

Player Modes
------------
- Human/Human
- AI/Human (Only for 2-player mode)

Game Modes
----------
- 2-Player
- 4-Player (Requires two PS4 controllers)

Puck Speed | Speed
---------- | -----
Speed 1 | Slow
Speed 2 | Normal
Speed 3 | Fast

Themes
------
- Classic
- Pokemon

Controls
--------
Mouse Control             | Action
------------------------- | ------
Hold left button and move | Move Player 2 paddle
Right-Click               | Open menu
Scroll Wheel Up           | Zoom camera in
Scroll Wheel Down         | Zoom camera out

Other Keyboard Input | Action
-------------------- | ------
'x'                  | Play/Pause song
Spacebar             | Play next song
Up Arrow Key         | Move camera up
Left Arrow Key       | Move camera left
Down Arrow Key       | Move camera down
Right Arrow Key      | Move camera right
Esc                  | Exit game

Keyboard Input for Player 1 | Action
--------------------------- | ------
'a' | Move paddle left
'w' | Move paddle up
's' | Move paddle down
'd' | Move paddle right
'q' | Move paddle up-left
'e' | Move paddle up-right
'z' | Move paddle down-left
'c' | Move paddle down-right

Keyboard Input for Player 2 | Action
--------------------------- | ------
'j' | Move paddle left
'i' | Move paddle up
'k' | Move paddle down
'l' | Move paddle right
'u' | Move paddle up-left
'o' | Move paddle up-right
'n' | Move paddle down-left
',' | Move paddle down-right

PS4 Controller Input | Action
-------------------- | ------
Controller 1 Left Analog Stick | Move Player 1 paddle
Controller 1 Right Analog Stick | Move Player 3 paddle
Controller 2 Left Analog Stick | Move Player 2 paddle
Controller 2 Right Analog Stick | Move Player 4 paddle

Extra Credit
------------
- Human/Human and AI/Human player modes
- Toggle for AI/Player2 control
- Game audio/music
- Themes: Classic and Pokemon
- PS4 Controllers
- 4-player mode
- Multiple difficulties (varying puck speeds)

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
The executable will be put in the bin directory. To run Air Hockey:
>$ cd bin

>$ ./AirHockey



