#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <GL/freeglut.h> 
#include <iostream>
#include <chrono>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier
#include <fstream>
#include "shaderClass.h"
#include "shaderClass.cpp"




//--Data types
//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

//--Evil Global variables
//Just for this example!
int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint vbo_geometry;// VBO handle for our geometry
int planet_orbit = -1;
int planet_rotation = -1;
int moon_orbit = -1;
int moon_rotation = -1;
bool object_rotation = false;// start the object as NOT rotating


// Even More Evil global constants
const char* vsFile = "../bin/shader.vs";
const char* fsFile = "../bin/shader.fs";

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position;
GLint loc_color;

//transform matrices
glm::mat4 planetModel;//obj->world each object should have its own model matrix
glm::mat4 moonModel;
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 planetMvp;//premultiplied modelviewprojection
glm::mat4 moonMvp;




//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
void myMouse (int button, int state, int x, int y);
void demo_menu(int id);
void idle();
void rotation_menu (int id);
void top_menu (int id);
void leftMouseClickMenu(int id);
void anotherKeyboard (int key, int x_pos, int y_pos);

//--Resource management
bool initialize();
void cleanUp();

//--Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;


//--Main
int main(int argc, char **argv)
{
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("Matrix Example");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }





    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input
    glutMouseFunc(myMouse); // Called if there is a mouse input
    glutSpecialFunc(anotherKeyboard);

    //Create the menu and submenu as described in book
    int sub_menu = glutCreateMenu(rotation_menu);//creates the rotation menu
    glutAddMenuEntry("start rotation", 2);// creates start rotation option
    glutAddMenuEntry("stop rotation", 3);// creates stop rotation option
    glutCreateMenu(top_menu);// this is the top menu
    glutAddMenuEntry("Quit", 1);// creates the option to quit
    glutAddSubMenu("start/stop rotation", sub_menu);// link to rotation menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);// right mouse button 



    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();
    if(init)
    {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}




//--Implementations
void render()
{
    //--Render the scene

    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //premultiply the matrix for this example
    planetMvp = projection * view * planetModel;
    moonMvp = projection * view * moonModel;

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(planetMvp));
    

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count


    // I was stuck on this for a while 
    // the moon has to be drawn seperately after planet is drawn
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(moonMvp));

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);
                           
    //swap the buffers
    glutSwapBuffers();
}

void update()
{
    //total time
    static float planetRotationAngle = 0.0;
    static float planetOrbitAngle = 0.0;
    static float moonRotationAngle = 0.0;
    static float moonOrbitAngle = 0.0;
    float dt = getDT();// if you have anything moving, use dt.

    // Lets first take care of the planet's orbit 
    if (planet_orbit == 1)
        {
        planetOrbitAngle += dt * M_PI/2; //move through 90 degrees a second
        }

    if (planet_orbit == -1)
        {
        planetOrbitAngle -= dt * M_PI/2; //move through 90 degrees a second
        }

    //Now we can take care of the moon's orbit
    if (moon_orbit == 1)
        {
        moonOrbitAngle += dt * M_PI/2; //move through 90 degrees a second
        }

    if (moon_orbit == -1)
        {
        moonOrbitAngle -= dt * M_PI/2; //move through 90 degrees a second
        }

    // next we must take care of the planet's rotation
    if (planet_rotation == 1)
        {
        planetRotationAngle += dt * M_PI/2; //move through 90 degrees a second
        }

    if (planet_rotation == -1)
        {
        planetRotationAngle -= dt * M_PI/2; //move through 90 degrees a second
        }

    // last, we must take care of the moon's rotation
    if (moon_rotation == 1)
        {
        moonRotationAngle += dt * M_PI/2; //move through 90 degrees a second
        }

    if (moon_rotation == -1)
        {
        moonRotationAngle -= dt * M_PI/2; //move through 90 degrees a second
        }

    // Now we must distinguish between planet and moon

    // first we set the planet's orbit
    planetModel = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(planetOrbitAngle), 0.0, 4.0 * cos(planetOrbitAngle)));

    // added for the rotation of planet around itself
    planetModel = glm::rotate( planetModel, (planetRotationAngle*3), glm::vec3(0.0f, 1.0f, 0.0f));

    // next we can set the moon's orbit and rotation but
    // we must make sure to set the moon's roation to the
    // position of the planet
    glm::vec3 positionOfPlanet = glm::vec3 (planetModel[3][0], planetModel[3][1], planetModel[3][2]);// this gives the planet's coordinates

    // set the moon's orbit around our planet
    moonModel = glm::translate( glm::mat4(1.0f), positionOfPlanet);

    moonModel = glm::translate( moonModel, glm::vec3(4.0 * sin(moonOrbitAngle), 0.0, 4.0 * cos(moonOrbitAngle)));

    moonModel = glm::scale (moonModel, glm::vec3(.5));

    // set the moon's rotation around itself
    moonModel = glm::rotate( moonModel, moonRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}


void reshape(int n_w, int n_h)
{
    w = n_w;
    h = n_h;
    //Change the viewport to be correct
    glViewport( 0, 0, w, h);
    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);

}

void keyboard(unsigned char key, int x_pos, int y_pos)
{
    // Handle keyboard input
    if(key == 27)//ESC
    {
        exit(0);
    }

    if((key == 'A') || (key == 'a'))//reverse planet rotation
        {
        planet_rotation *= -1;
        }

    if((key == 'Q') || (key == 'q'))//reverse moon rotation
        {
        moon_rotation *= -1;
        }

    if((key == 'S') || (key == 's'))//reverse planet orbit
        {
        planet_orbit *= -1;
        }

    if((key == 'W') || (key == 'w'))//reverse moon orbit
        {
        moon_orbit *= -1;
        }   
}


void anotherKeyboard (int key, int x_pos, int y_pos)
{
    if(key == GLUT_KEY_RIGHT)
        {
         planet_rotation *= -1;
        }
    else if(key == GLUT_KEY_LEFT)
        {
         planet_orbit  *= -1;
        }
}


void myMouse (int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)// present rotation and orbit options upon left mouse click
       {
       glutCreateMenu(leftMouseClickMenu);
       glutAddMenuEntry("Quit", 1);
       glutAddMenuEntry("reverse planet rotation", 2);
       glutAddMenuEntry("reverse planet orbit", 3);
       glutAddMenuEntry("reverse moon rotation", 4);
       glutAddMenuEntry("reverse moon orbit", 5);
       glutAttachMenu(GLUT_LEFT_BUTTON);
       }
}


void leftMouseClickMenu(int id)
    {
    switch(id)
        {
        case 1:// quit the program
        glutLeaveMainLoop();
        break;
        case 2:// rotate planet in opposite direction
        planet_rotation *= -1;
        break;
        case 3:// change planet orbit to opposite direction
        planet_orbit *= -1;
        break;
        case 4:
        moon_rotation *= -1;// change rotation of moon
        break;
        case 5:
        moon_orbit *= -1;// chang orbit of moon
        break;
        }
        glutPostRedisplay();
    }


void rotation_menu (int id)
{
switch (id)
    {
    case 2:
    object_rotation = true;// rotate object
    break;
    case 3:
    object_rotation = false;// stop object rotation
    break;    
    }
    glutPostRedisplay();
}


void top_menu (int id)
{
switch (id)
    {
    case 1:
    glutLeaveMainLoop();// quit the program
    break;   
    }
    glutPostRedisplay();
}



void idle()
{
glutPostRedisplay();
}

bool initialize()
{
    // Initialize basic geometry and shaders for this example

    //this defines a cube, this is why a model loader is nice
    //you can also do this with a draw elements and indices, try to get that working
    Vertex geometry[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };
    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

    //--Geometry done

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // check if shaders are created
        if(!vertex_shader)
    {
        std::cerr << "Error creating shader vertex!" << std::endl;
        return false;
    }

        if(!fragment_shader)
    {
        std::cerr << "Error creating shader fragment!" << std::endl;
        return false;
    }



    //Shader Sources
    // Put these into files and write a loader in the future
    // Note the added uniform!
    shaderLoaderClass loadVertex;
    shaderLoaderClass loadFragment;

    std::string vsString = loadVertex.loadShader (vsFile);
    std::string fsString = loadFragment.loadShader (fsFile);  

    const char *vs = vsString.c_str ();
    const char *fs = fsString.c_str (); 


    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);
    //check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
        return false;
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);
    //check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
        return false;
    }

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    //check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_color = glGetAttribLocation(program,
                    const_cast<const char*>("v_color"));
    if(loc_color == -1)
    {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }
    
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane, 

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //and its done
    return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry);
}

//returns the time delta
float getDT()
{
    float ret;
    // if object not rotating set time to t1
    if( !object_rotation ){
        t1 = std::chrono::high_resolution_clock::now();
    }
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}































