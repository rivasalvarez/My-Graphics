#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h>
#include <iostream>
#include <Magick++.h>
#include "CelestialBody.h"

// Evil Global variables
int w = 960, h = 720; // Window size
int NUM_OBJECTS = 0;
glm::vec3 eyePosition = {0.0, 0.0, 0.0};
glm::vec3 eyeOffset = {40.0, 40.0, 40.0};
int focusObjIndex = 0;
CelestialBody** obj;
glm::mat4 view;
glm::mat4 projection;
bool info = true;

// GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);

// Other functions
bool initialize();
void keyboard(unsigned char key, int x_pos, int y_pos);
void specialKey(int key, int x_pos, int y_pos);
void mouse(int button, int state, int x, int y);
void menu(int option);

// Time stuff
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
      
// Main
int main(int argc, char **argv) {
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

    // Create window
    glutInitWindowSize(w, h);
    glutCreateWindow("Solar System");
    glutFullScreen();
    
    // Create menu
    glutCreateMenu(menu);
    glutAddMenuEntry("Start/Pause", 1);
    glutAddMenuEntry("Change Scale", 2);
    glutAddMenuEntry("Quit", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Initialize Glew
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
    glutMouseFunc(mouse);// Called if there is mouse input
    glutSpecialFunc(specialKey);// Called if there is special key input

    // Initialize all of our resources(shaders, geometry)
    bool init;
    init = initialize();

    // Start glut main loop if successfully initialized
    if(init) {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    return 0;
}

// Callback function that renders the scene and celestial body objects
void render() {
    // Clear the screen
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Print celestial body info if it exists
    if(info) {
      obj[focusObjIndex]->printInfo();
    }

    // Calculate projection * view premultiplication
    glm::mat4 vp = projection * view;
    
    // Render shader objects
    for(int i=0; i<NUM_OBJECTS; i++) {
       obj[i]->render(vp);
    }
    
    // Draw orbit paths for planets
    for(int i=1; i<10; i++) {
      obj[i]->drawOrbit(vp);
    }
    
    // Swap the buffers
    glutSwapBuffers();
}

// Callback function that updates the scene with object transformations and camera view
void update() {
   // Get total time
   float dt = getDT();
    
   // Update Sun
   obj[0]->update(dt, glm::mat4(1.0f), "Sun");

   // Update celestial bodies
   for(int i=1; i<NUM_OBJECTS; i++)
   {
      int index = obj[i]->getObjIndex();
      obj[i]->update(dt, obj[index]->getTranslateModel(), obj[index]->getObjName());
   }

   // Update camera view
   glm::mat4 model = obj[focusObjIndex]->getModel();
   eyePosition = { model[3][0], model[3][1], model[3][2] };

   // Update camera view
   view = glm::lookAt( glm::vec3(eyePosition.x + eyeOffset.x, eyePosition.y + eyeOffset.y, eyePosition.z + eyeOffset.z), //Eye Position
                       eyePosition, //Focus point
                       glm::vec3(0.0, 1.0, 0.0));   
}


// Reshape CelestialBody object based on new window size
void reshape(int n_w, int n_h) {
    w = n_w;
    h = n_h;

    //Change the viewport to be correct
    glViewport( 0, 0, w, h);

    // Update the projection matrices for shader objects
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100000.0f);
}

// Initializes celestial body objects and planetary information
bool initialize() {
    bool result = true;
    std::ifstream fin;
    fin.open("objectNames");

    // Read in number of objects and allocate memory
    fin >> NUM_OBJECTS;
    std::string nameArray[NUM_OBJECTS];
    obj = new CelestialBody* [NUM_OBJECTS];

    // Read in names and close file
    for(int i = 0; i < NUM_OBJECTS; i++)
      fin >> nameArray[i];
    fin.close();

    // Set view and projection matrices
    view = glm::lookAt( glm::vec3(eyePosition.x + eyeOffset.x, eyePosition.y + eyeOffset.y, eyePosition.z + eyeOffset.z), //Eye Position
                        eyePosition, //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); 

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                  float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                  0.01f, //Distance to the near plane, normally a small value like this
                                  100000.0f); //Distance to the far plane,

    // Initialize shader objects
    for(int i=0; i<NUM_OBJECTS; i++){
      // Create shader object
      obj[i] = new CelestialBody(nameArray[i]);

    fin.close();
    
    // Initialize celestial objects
    bool isSuccess = obj[i]->initialize(w, h);

      // Check if initialization successful, do not start glut main loop if false
      if(!isSuccess) result = false;
    }

    // Initialize Planet Info
    fin.open("PlanetInfo");
    int numStr = 0;
    std::string dummy;
    std::string* temp;

    // Iterate through Sun and 9 planets
    for(int j = 0; j < 10; j++) {
        fin >> numStr;       //Read in number of strings
        getline(fin,dummy);  //Read in whitespace (really this was a problem)

        temp = new std::string[numStr];

        for(int z = 0; z < numStr; z++) {
            getline(fin, temp[z]);
        }

        // Give strings to Classes and delete memory
        obj[j]->initInfo(numStr, temp);
        delete[] temp;
    }

    fin.close();
    return result;
}

// Callback function for keyboard input (change focus point, quit program, pause/resume solar system, toggle information)
void keyboard(unsigned char key, int x_pos, int y_pos) {
    // Handle keyboard input
    switch(key) {
       // Esc, quit program
       case 27:
          exit(0);
          break;
 
       // Spacebar, pauses/unpauses rotations and orbits
       case 32:
          for(int i=0; i<NUM_OBJECTS; i++)
             obj[i]->pause();
             break;
             
       // Change focus point to Mercury
       case '1':              
             focusObjIndex = 1;
             break;

       // Change focus point to Venus
       case '2':              
             focusObjIndex = 2;
             break;
             
       // Change focus point to Earth
       case '3':              
             focusObjIndex = 3;
             break;
             
       // Change focus point to Mars
       case '4':             
             focusObjIndex = 4;
             break;
             
       // Change focus point to Jupiter
       case '5':              
             focusObjIndex = 5;
             break;
             
       // Change focus point to Saturn
       case '6':              
             focusObjIndex = 6;
             break;
             
       // Change focus point to Uranus
       case '7':              
             focusObjIndex = 7;
             break;
             
       // Change focus point to Neptune
       case '8':              
             focusObjIndex = 8;
             break;
             
       // Change focus point to Pluto
       case '9':              
             focusObjIndex = 9;
             break;
             
       // Change focus point to Sun
       case '0':              
             focusObjIndex = 0;
             break;    

       // Show/Hide info
       case 'i':
             info = !info;
             break;       
               
       default:
          break;
    }

   // Reset camera offsets
   eyeOffset = {40.0, 40.0, 40.0};
}

// Callback function for arrow keys
void specialKey(int key, int x_pos, int y_pos) {
    // Handle keyboard input
    switch(key) {
       // Increase view on y-axis
       case GLUT_KEY_UP:
          if(eyeOffset.y < 4500.0)
               eyeOffset.y = std::min(eyeOffset.y + 20.0, 4500.0);
          break;
       
       // Decrease view on y-axis
       case GLUT_KEY_DOWN:
          if(eyeOffset.y > -4500.0)
               eyeOffset.y = std::max(eyeOffset.y - 20.0, -4500.0);
          break;

       default:
          break;
    }
}

// Callback function for mouse input (change view, zoom in/out)
void mouse(int button, int state, int x, int y) {
   // Angle used to calculate new y position if z-offset moved
   double initialAngle = atan(eyeOffset.y / eyeOffset.z);
   
   // Left-click, toggle solar system view
   if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      for(int i=0; i<NUM_OBJECTS; i++) {
         obj[i]->toggleView();
      }
   }
   
   // Wheel scroll up to zoom in
   if(state == GLUT_UP && button == 3) {
      float zOffset = eyeOffset.z - 20.0;
      float yOffset = tan(initialAngle) * zOffset;
      
      if(zOffset > .001 && (yOffset < 5000.0 && yOffset > -5000.0)) {
         eyeOffset.z = zOffset;
         eyeOffset.y = yOffset;
      }
   }

   // Wheel scroll down to zoom out
   else if(state == GLUT_UP && button == 4) {
      float zOffset = eyeOffset.z + 20.0;
      float yOffset = tan(initialAngle) * zOffset;

      if(zOffset < 5000.0 && (yOffset < 5000.0 && yOffset > -5000.0)) {
         eyeOffset.z = zOffset;
         eyeOffset.y = yOffset;
      }
   }
}

// Menu commands (Start/pause solar system, toggle view, exit program)
void menu(int option) {
    switch(option) {
      // Start/Pause solar system
      case 1:
         for(int i=0; i<NUM_OBJECTS; i++)
            obj[i]->pause();
         break;
         
      // Toggle view
      case 2:
         for(int i=0; i<NUM_OBJECTS; i++)
            obj[i]->toggleView();
         break;
         
      // Exit program
      case 3:
         exit(0);
         break;

      default:
         break;
    }
    glutPostRedisplay();
}

// Returns the time delta
float getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

