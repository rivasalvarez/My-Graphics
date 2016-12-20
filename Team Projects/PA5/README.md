Programming Assignment 05
=========================
Team Members
------------
Marco Rivas, Martin Revilla, Truman Chan

Description
-----------
Objective of this assignment was to use Assimp to load models (and then display them)

Program accepts an argument for .obj filename. If one is not provided, it will use the default board.obj that I created in Blender. An example of executing the program would be:

      ./CS480 board.obj

Extra Credit
------------
Loads colors of objects - uses diffuse components of materials from materials file. The materials file name is taken from the "mtllib [file name]" section of the object file.

Building This Example
---------------------

To build this example:

>$ cd build
>$ make

The executable will be put in the bin directory.

To clean this example (remove object files and executables):

>$ cd build
>$ make clean

