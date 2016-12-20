Programming Assignment 10
=========================
Team Members
------------
Marco Rivas, Martin Revilla, Truman Chan

Description
-----------

The purpose of this assignment is to create a program that applies 4 lighting types to a single model. To accomplish this you will be using shaders and the Phong Model to apply the correct lighting.

Keyboard Input | Action
---------------|-------
'a' | Toggle Ambient Lighting

Extra Credit
------------


Building This Assignment
---------------------

This assignment uses Magick++ to load its images. To install Magick++:
>$ sudo apt-get install libmagick++-dev

This assigment also uses Bullet. To install:
>$ sudo apt-get install libbullet-dev

To build this assignment:

>$ cd build

>$ make

The executable will be put in the bin directory.

To clean this example (remove object files and executables):

>$ cd build

>$ make clean

Issues
------
Adding distant, point, and spotlights caused issues with rendering object (object would disappear). Was not resolved, and code was reverted to include ambient lighting. 
