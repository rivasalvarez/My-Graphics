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
void themeMenu(int option);
void playerMenu(int option);
void gameMenu(int option);
void puckMenu(int option);
void menu(int option);
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
    glutCreateWindow("Air Hockey");
    glutFullScreen(); 
    
    // Create theme submenu
    int theme = glutCreateMenu(themeMenu);
    glutAddMenuEntry("Classic", 1);
    glutAddMenuEntry("Pokemon", 2);
    
    // Create player mode submenu
    int playerMode = glutCreateMenu(playerMenu);
    glutAddMenuEntry("Human/Human", 1);
    glutAddMenuEntry("AI/Human", 2);
    
    // Create game mode submenu
    int gameMode = glutCreateMenu(gameMenu);
    glutAddMenuEntry("2-Players", 1);
    glutAddMenuEntry("4-Players", 2);
    
    // Create puck submenu
    int puckSpeed = glutCreateMenu(puckMenu);
    glutAddMenuEntry("Speed 1", 1);
    glutAddMenuEntry("Speed 2", 2);
    glutAddMenuEntry("Speed 3", 3);
    
    // Create menu
    glutCreateMenu(menu);
    glutAddMenuEntry("New Game", 1);
    glutAddMenuEntry("Pause/Resume", 2);
    glutAddSubMenu("Themes", theme);
    glutAddSubMenu("Player Mode", playerMode);
    glutAddSubMenu("Game Mode", gameMode);
    glutAddSubMenu("Puck Speed", puckSpeed);
    glutAddMenuEntry("Quit", 3);
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

// Callback function for theme menu interation
void themeMenu(int option) {
   gameEngine.themeMenu(option);
}

// Callback function for player mode menu interation
void playerMenu(int option) {
   gameEngine.playerMenu(option);
}

// Callback function for game mode menu interation
void gameMenu(int option) {
   gameEngine.gameMenu(option);
}

// Callback function for puck speed menu interation
void puckMenu(int option) {
   gameEngine.puckMenu(option);
}

// Keyboard input to move paddle 1
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

// Mouse control for paddle 2
void mouseMovement(int x, int y) {
   gameEngine.mouseMovement(x, y);
}

// Initialize shader objects and view/projection matrices
bool initialize() {
   return gameEngine.initialize();
}

