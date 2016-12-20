Programming Assignment 06
=========================
Team Members
------------
Marco Rivas, Martin Revilla, Truman Chan

Description
-----------
Objective of this assignment was to use Assimp to load textures (and then display them)

Program accepts an argument for .obj filename, and an argument for the texture. If one is not provided, it will use the default capsule.obj or capsule0.jpg. An example of executing the program would be:

      ./CS480 capsule.obj capsule0.jpg


Building This Assignment
---------------------

This assignment uses Magick++ to load its images. To install Magick++:
>$ sudo apt-get install libmagick++-dev

To build this assignment:

>$ cd build

>$ make

The executable will be put in the bin directory.

To clean this example (remove object files and executables):

>$ cd build

>$ make clean

