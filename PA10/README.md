Lighting
========

Project Authors
---------------
This is the repository for the group Catherine Pollock, Conor Sullivan, and Peter Rahmanifar

Program Function
----------------
The program models a table with interactive objects. Various types of lighting can be shown on the objects. An information file can be passed through the command line, or default ones will be used. Object information file must be in the following format:

```
int numberOfObjects
string filepathToObject1
string filepathToTexture1
string filepathToObject2
string filepathToTexture2
...
```

Information file must be in correct format or undesired results will occur. All files mentioned must be in location specified in info file. 

When the program is running, the following commands can be used:

- To quit: *Esc*
- To interact with cylinder: *Arrow keys*
- To interact with the sphere: *WASD*
- To change lighting type: *1 2 3 4*

Project discripton found here: http://www.cse.unr.edu/~fredh/class/480/F2015/proj/PA10/PA10.php

Extra Credit
------------
No extra credit achieved.

Ubuntu Dependencies
-------------------
The following packages can be installed with these commands:

*This project requires bullet* 

>$ sudo apt-get install libbullet-dev

*This project requires magick++* 

>$ sudo apt-get install libmagick++-dev imagemagick

*This project requires assimp3* 

>$ sudo apt-get install libassimp-dev

*This project requires GLUT and GLEW* 

>$ sudo apt-get install freeglut3-dev freeglut3 libglew1.6-dev

*This project requires GLM*

>$ sudo apt-get install libglm-dev

Mac OSX Dependencies
--------------------

The following libraries can be installed with these commands using homebrew:

*This project requires bullet*

>$ brew install bullet

*This project requires magick++*

>$ brew install imagemagick

*This project requires assimp3* 

>$ brew install assimp

*This project requires GLUT and GLEW* 

>$ brew install freeglut glew

*This project requires GLM*

>$ brew install glm

Additionally, ensure that the latest version of the Developer Tools is installed

Building this Project
---------------------

To build this project just 

>$ cd build

>$ make

If you are using a Mac you will need to edit the makefile in the build directory

The excutable will be put in bin. The program must be ran from inside build or bin.

To run the program, execute these commands, replacing with your object filepath and your object's texture filepath:

>$ cd ../bin/

>$ ./Model

*or*

>$ cd ../bin/

>$ ./Model ./objectinfo.txt 
