Programming Assignment 08
=========================
Team Members
------------
Marco Rivas, Martin Revilla, Truman Chan

Description
-----------

Objective of this assigment is to build a simple board with a bottom and side walls, a sphere, a cube, and a cylinder that detect collision. The cube should be a static object and the sphere and cylinder should be dynamic objects. There should be keyboard input so that the dynamic objects can move.

Keyboard Input | Action
-------------- | ------
'a' | Move sphere left
'd' | Move sphere right
'w' | Move sphere up
's' | Move sphere down
'j' | Move cylinder left
'l' | Move cylinder right
'i' | Move cylinder up
'k' | Move cylinder down

Extra Credit
------------

Loaded a triangle mesh into bullet for the table.

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

