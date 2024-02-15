#Ricky and Ethan's CS107E Final Project: Rubik's Cube Robot Solver

For our final project, we decided to build a Rubik's Cube solver. 
The robot used six stepper motors to control each of the six faces of the cube. 
The stepper motors were driven by stepper motor drivers. 
As for the rig itself, the frame was constructed using plexiglass on which we mounted the motors.
We used the PS/2 keyboard to control the motors with the corresponding letters. 

As for the solving algorithm, we decided to use the 3-style method which is an advanced blindfolded method.
The method is based on algorithms called commutators, which each cycle 2 pieces into their correct spots at a time.
By analyzing the state of the scrambled cube, we were able to determine the solution by concatenating the commutators needed to put every piece in the right spot.

We also used the graphics library to draw the state of the cube on the monitor for better visibility of the cube during the scramble.
The final solve time is also displayed at the end of the solve.

This application can also be run without the cube and the motors; the program will still display the cube and produce the solution to solve it.

In the future, we are considering adding an option to input the colors of the cube by hand, where the program just gives the solution which will allow you to solve a physical scrambled Rubik's Cube if you are unable to by yourself.
