A simple example of matrix use in OpenGL
========================================

Explanation of the assignment from <http://www.cse.unr.edu/~fredh/class/480/F2015/proj/PA02/PA2.php>:

 Objective: Now that we have a window open and an animation going, we will be adding interaction. You'll be taking your code from last week and adding menus, keyboard interaction, and mouse interaction.

    Menus: To learn more about adding menus, look in your book at Section 2.12 Menus. You'll be adding an option to start the spinning, to pause the spinning, and to exit the program.
        A hint on starting/pausing the rotation: add a flag that is checked when the cube is rotated. 
    Keyboard: To learn more about adding keyboard events, look in the book at Section 2.11.3 Keyboard Events. You will need to change the spinning of your cube based upon a key being pressed. For example if you press A, the rotation reverses direction.
    Mouse: Look in Section 2.11.1 Using the Pointing Device. You will be replicating what we did with the keyboard by using the mouse. If you click on the screen, the cube will reverse rotation. 

Libraries being used:
-All libraries being used are the same from PA1: 
#include <GL/glew.h>
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <iostream>
#include <chrono>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier
#include <fstream>
#include "shaderClass.h"
#include "shaderClass.cpp"

How to run the program:
-Once make is run in the build folder, a Matrix program will appear in the bin folder. In order to execute, double click on the program or right click and choose the "Run" option. A screen will appear with a stationary cube. If you right click, a menu will appear that will have a "Quit" option, which will end the program, and a "start/stop rotation" option. When the latter is highlighted, a submenu will appear with an option to either "start rotation"
or "stop rotation". The former of the two will initiate the rotation of the planet in its orbit. The latter will freeze the object on the screen. The left mouse click reveals a menu with three options: "Quit", "reverse rotation", and "reverse orbit". "Quit" simply ends the program. The "reverse rotation" option reverses the rotation of the object about its own y-axis. The "reverse orbit" option reverses the direction of the orbit the object is currently going in. These opertions can also be done with the keyboard. The rotation of the object about its y-axis can be reversed by pressing 'A' or 'a' on the keyboard. Pressing 'B' or 'b' on the keyboard will reverse the orbit of the object. All reversals can be applied whether the object is moving or is still. Pressing 'esc' on the keyboard will end the program.


Building This Example
---------------------

*This example requires GLM*
*On ubuntu it can be installed with this command*

>$ sudo apt-get install libglm-dev

*On a Mac you can install GLM with this command(using homebrew)*
>$ brew install glm

To build this example just 

>$ cd build
>$ make

*If you are using a Mac you will need to edit the makefile in the build directory*

The excutable will be put in bin

Additional Notes For OSX Users
------------------------------

Ensure that the latest version of the Developer Tools is installed.
