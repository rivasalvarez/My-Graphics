#define GLM_FORCE_RADIANS 
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <iostream>
#include <chrono>
#include "GameEngine.h"

int w = 640, h = 480; // Window size
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
GameEngine gameEngine;

void render();
void reshape(int, int);
void update();
void menu(int option);
void lightingMenu(int option);
void keyboard(unsigned char, int, int);
void specialKey(int, int, int);
void mouse(int button, int state, int x, int y);
void mouseMovement(int x, int y);
bool initialize();

// Main
int main(int argc, char **argv) {
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutCreateWindow("Labyrinth");
    glutFullScreen(); 

    // Create lighting submenu
    int lighting = glutCreateMenu(lightingMenu);
    glutAddMenuEntry("Ambient", 1);    
    glutAddMenuEntry("Ball Spotlight", 2);
    glutAddMenuEntry("Goal Spotlights", 3);
    
    // Create menu
    glutCreateMenu(menu);
    glutAddMenuEntry("New Game", 1);
    glutAddMenuEntry("Pause/Resume", 2);
    glutAddSubMenu("Lighting", lighting);
    glutAddMenuEntry("Display Scoreboard", 3);
    glutAddMenuEntry("Quit", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Initialize GLEW
    GLenum status = glewInit();
    if(status != GLEW_OK) {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        return -1;
    }

    // Set callbacks for GLUT
    glutDisplayFunc(render); // Display objects
    glutReshapeFunc(reshape); // Resize window
    glutIdleFunc(update); // Called if there is nothing else to do
    glutKeyboardFunc(keyboard); // Keyboard input
    glutSpecialFunc(specialKey); // Arrow key input
    glutMouseFunc(mouse); // Mouse input
    glutMotionFunc(mouseMovement); // Track mouse movement when button pressed
   
    // Initialize all of our resources (shaders, geometry)
    bool init = gameEngine.initialize();

    // Start glut main loop if successfully initialized
    if(init) {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    return 0;
}

// Callback function that renders the scene and shader objects
void render() {
   gameEngine.render();
}

// Callback function that reshapes objects based on window size
void reshape(int n_w, int n_h) {
   gameEngine.reshape(n_w, n_h);
}

// Callback function that updates object in clip space
void update() {
   gameEngine.update();
}

// Callback function for menu interation
void menu(int option) {
   gameEngine.menu(option);
}

// Callback function for menu interation
void lightingMenu(int option) {
   gameEngine.lightingMenu(option);
}

// Keyboard input to move board
void keyboard(unsigned char key, int x_pos, int y_pos) {
   gameEngine.keyboard(key, x_pos, y_pos);
}

// Arrow key input to move camera
void specialKey(int key, int x_pos, int y_pos) {
   gameEngine.specialKey(key, x_pos, y_pos);
}

// Mouse input to control paddle 2 and interact with menu
void mouse(int button, int state, int x, int y) {
   gameEngine.mouse(button, state, x, y);
}

// Mouse control for table
void mouseMovement(int x, int y) {
   gameEngine.mouseMovement(x, y);
}

// Initialize shader objects and view/projection matrices
bool initialize() {
   return gameEngine.initialize();
}

