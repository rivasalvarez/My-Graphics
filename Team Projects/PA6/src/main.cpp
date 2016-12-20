#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <iostream>
#include <Magick++.h>
#include <btBulletDynamicsCommon.h>

#include "shaderClass.h"

// Evil Global variables
int w = 640, h = 480;// Window size
Shader *shaderObj;

// GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);

bool initialize(const char *objFilename, const char *textFilename);
void keyboard(unsigned char key, int x_pos, int y_pos);

// Main
int main(int argc, char **argv) {
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

    // Create window
    glutInitWindowSize(w, h);
    glutCreateWindow("PA 06");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if(status != GLEW_OK) {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input

    // Initialize all of our resources(shaders, geometry)
    bool init;
    if(argc == 2)
       init = initialize(argv[1], "capsule0.jpg");

    else if(argc > 2)
       init = initialize(argv[1], argv[2]);
    
    else
       init = initialize("capsule.obj","capsule0.jpg");
    
    // Start glut main loop if successfully initialized
    if(init) {
        glutMainLoop();
    }

    return 0;
}

// Implementations
void render() {
    //--Render the scene

    // Clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render shader objects
    shaderObj->render();

    // Swap the buffers
    glutSwapBuffers();
}

void update() {
    // Rotate
    shaderObj->update();
}

void reshape(int n_w, int n_h) {
    w = n_w;
    h = n_h;

    //Change the viewport to be correct
    glViewport( 0, 0, w, h);

    // Update the projection matrices for shader objects
    shaderObj->reshape(w, h);

}

bool initialize(const char *objFilename, const char *textFilename) {
   // Create shader object
   shaderObj = new Shader();
      
   // Initialize
   bool result = shaderObj->initialize(objFilename, textFilename, w, h);
   
   return result;
}

void keyboard(unsigned char key, int x_pos, int y_pos) {
    // Handle keyboard input
    switch(key) {
       // Esc, quit program
       case 27:
          exit(0);
          break;

       default:
          break;
    }
}


