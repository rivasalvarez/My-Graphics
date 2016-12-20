Programming Assignment 07: Solar System
=========================
Team Members
------------
Marco Rivas, Martin Revilla, Truman Chan

Description
-----------
Make a solar system simulation.

Mouse Control | Action 
------------- | ------
Left-Click | Switch between actual data and scaled view of celestial bodies
Right-Click | Open menu
Scroll Wheel Up | Zoom camera in
Scroll Wheel Down | Zoom camera out
      
Keyboard Input | Action
-------------- | ------
'1' | Change view to Mercury
'2' | Change view to Venus
'3' | Change view to Earth
'4' | Change view to Mars
'5' | Change view to Jupiter
'6' | Change view to Saturn
'7' | Change view to Uranus
'8' | Change view to Neptune
'9' | Change view to Pluto
'0' | Change view to Sun
'i' | Toggle planetary information
'Spacebar' | Start/Pause rotations and orbits of celestial bodies
'Esc' | Quit program
   
Special Key Input | Action
----------------- | ------
Up Arrow | Tilt camera up y-axis
Down Arrow | Tilt camera down y-axis

Extra Credit
------------

- Add option to switch between actual data and scaled view
- Draw orbit paths of planets
- Display information about celestial bodies
- Accurate orbit path for Pluto
- Proper planet rotations (Venus rotates clockwise, Uranus along y-axis)

Building This Example
---------------------

To build this example:

>$ cd build
>$ make

The executable will be put in the bin directory.

To clean this example (remove object files and executables):

>$ cd build
>$ make clean

