#include "GameEngine.h"
#include "MusicPlayer.h"

// Constructor for game engine
GameEngine::GameEngine(){
      // Game Engine members
      isPaused = false;
      isMusicPaused = true;
      gameIsFinished = false;
      displayTimes = true;
      victoryFrames = 0;
      startTime = std::chrono::high_resolution_clock::now();
      currentPauseDuration = 0.0;
      totalPauseDuration = 0.0;

      // Boundaries and camera angle changes
      angleChange = M_PI / 32;
      leftAngleBound = -M_PI / 4;
      rightAngleBound = M_PI / 4;
      topAngleBound = 15 * M_PI / 32;
      bottomAngleBound = -M_PI / 8;
      
      // Lighting variables
      ambientLight = true;
      ballLight = true;
      goalLight = true;
}

// Destructor for game engine
GameEngine::~GameEngine(){

}

// Callback function that renders the scene and shader objects
void GameEngine::render() {
   graphics.render();
}


// Callback function that updates the scene with object transformations and camera view
void GameEngine::update() {
    if(isPaused) {
       currentPauseDuration = getPauseDuration();
    }
    
    gameController.update();
    physicsEngine.update();
    musicPlayer.update();
    graphics.update();
}

// Reshape objects based on new window size
void GameEngine::reshape(int nw, int nh) {
    graphics.reshape(nw, nh);
}

// Initialize shader objects and view/projection matrices
bool GameEngine::initialize() {
    graphics.initialize(this);
    musicPlayer.initialize(this);
    physicsEngine.initialize(this, &graphics);
    gameController.initialize(this, &graphics);

    // Read in times
    readInTimes();
    return true;
}

// Callback function that handles keyboard input
void GameEngine::keyboard(unsigned char key, int x_pos, int y_pos) {
   // If the game is finished, reset booleans to false. NOTE: if a keyboard was pressed, the victory notification is cleared
    if(gameIsFinished) {
       gameIsFinished = false;
    }
    
    // Handle keyboard input
    switch(key) {
       // Esc, quit program
       case 27:
          exit(0);
          break;

       // Pause the game music
       case 'x':
          if(isMusicPaused) {   
             isMusicPaused = false;
             musicPlayer.play();
          }

          else {  
             isMusicPaused = true;
             musicPlayer.pause();
          }
          break;

       // Next Song
       case 32:
          musicPlayer.playNext();
          break;
    }
}

// Special keyboard input to change camera view
void GameEngine::specialKey(int key, int x_pos, int y_pos) {
    // Handle keyboard input
    switch(key) {
       // Move camera view right
       case GLUT_KEY_RIGHT:
          graphics.xAxisRadians = std::max(graphics.xAxisRadians - angleChange, leftAngleBound);
          break;

       // Move camera view left
       case GLUT_KEY_LEFT:
          graphics.xAxisRadians = std::min(graphics.xAxisRadians + angleChange, rightAngleBound);
          break;

       // Move camera view down
       case GLUT_KEY_DOWN:
          graphics.yAxisRadians = std::max(graphics.yAxisRadians - angleChange, bottomAngleBound);
          break;
       
       // Move camera view up
       case GLUT_KEY_UP:
          graphics.yAxisRadians = std::min(graphics.yAxisRadians + angleChange, topAngleBound);
          break;

       default:
          break;
    }
             
    // Calculate eye position
    graphics.eyePosition.x = graphics.cameraRadius * cos(graphics.yAxisRadians) * sin(graphics.xAxisRadians);
    graphics.eyePosition.y = graphics.cameraRadius * cos(graphics.yAxisRadians) * cos(graphics.xAxisRadians);
    graphics.eyePosition.z = graphics.cameraRadius * sin(graphics.yAxisRadians);
}

void GameEngine::readInTimes() { 
    std::ifstream fin;
    fin.open("highScores.txt");
    //read in top 10
    for(int i = 0; i < 10; i++){
    fin >> bestTimes[i];
    }
    fin.close();
}


void GameEngine::checkHigh() {
    //Loop though hishscores
    for(int i = 0; i < 10; i++){
      // check if its a highscore
      if(finishTime < bestTimes[i]  && finishTime > 50){
      victoryFrames = 400;
      float temp = finishTime;
      finishTime = bestTimes[i];
      bestTimes[i] = temp;
      }
    }
}

void GameEngine::sendOutTimes()  {
    std::ofstream fout;
    fout.open("highScores.txt");
    //export top 10 when new highscore
    for(int i = 0; i < 10; i++){
    fout << bestTimes[i] << std::endl;
    }
    fout.close();
}

// Callback function for mouse input (change view, zoom in/out)
void GameEngine::mouse(int button, int state, int x, int y) {
    // If the game is finished, reset booleans to false. NOTE: if a keyboard was pressed, the victory notification is cleared
    if(gameIsFinished) {
       gameIsFinished = false;
    }

    // Left-click down, hold for paddle movement
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
       mouseX = x;
       mouseY = y;
    }
    
    // Wheel scroll up to zoom in
    if(state == GLUT_UP && button == 3 && graphics.cameraRadius > 10.0) {
       graphics.cameraRadius -= 10.0;
    }

    // Wheel scroll down to zoom out
    else if(state == GLUT_UP && button == 4 && graphics.cameraRadius < 70.0) {
       graphics.cameraRadius += 10.0;
    }
}

// Callback function that tracks mouse position when button held
void GameEngine::mouseMovement(int x, int y) {
    // Lower speed = faster movement
    int speed = 4;
    
    // Move table right
    if(x - mouseX < 0) {
       graphics.xAxisRadians = std::min(graphics.xAxisRadians + (angleChange / speed), rightAngleBound);
    }
    
    // Move table left
    else {
       graphics.xAxisRadians = std::max(graphics.xAxisRadians - (angleChange / speed), leftAngleBound);
    }
    
    // Move table down
    if(y - mouseY > 0) {
       graphics.yAxisRadians = std::max(graphics.yAxisRadians - (angleChange / speed), bottomAngleBound);
    }
    
    // Move table up
    else {
       graphics.yAxisRadians = std::min(graphics.yAxisRadians + (angleChange / speed), topAngleBound);
    }
    
    // Store new mouse location
    mouseX = x;
    mouseY = y;
}

// Menu commands
void GameEngine::menu(int option) {
    switch(option) {
      // Start New Game
      case 1:
         resetGame();
         break;
         
      // Pause/Resume
      case 2:
         isPaused = !isPaused;
         if(isPaused) {
            pauseStartTime = std::chrono::high_resolution_clock::now();
         }
         else {
            totalPauseDuration += currentPauseDuration;
            currentPauseDuration = 0;
         }

         break;
         
      // Display Scoreboard
      case 3:
         displayTimes = !displayTimes;
         break;
         
      // Quit Game
      case 4:
         exit(0);
         break;

      default:
         break;
    }
    glutPostRedisplay();
}

// Menu commands
void GameEngine::lightingMenu(int option) {
    switch(option) {
      // Ambient lighting
      case 1:
         ambientLight = !ambientLight;
         break;
         
      // Ball spotlight
      case 2:
         ballLight = !ballLight;
         break;
         
      // Goal spotlights
      case 3:
         goalLight = !goalLight;
         break;

      default:
         break;
    }
    glutPostRedisplay();
}

void GameEngine::resetGame() {
   gameIsFinished = false;
   isPaused = false;
   physicsEngine.cp1 = false;
   physicsEngine.cp2 = false;
   physicsEngine.cp3 = false;

   // Reset Camera
   graphics.cameraRadius = 20.0; // Distance from focus point
   graphics.xAxisRadians = 0; // Latitude
   graphics.yAxisRadians = graphics.tiltOffset; // Longitude
   
   // Calculate eye position
   graphics.eyePosition.x = graphics.cameraRadius * cos(graphics.yAxisRadians) * sin(graphics.xAxisRadians);
   graphics.eyePosition.y = graphics.cameraRadius * cos(graphics.yAxisRadians) * cos(graphics.xAxisRadians);
   graphics.eyePosition.z = graphics.cameraRadius * sin(graphics.yAxisRadians);
   
   // Reinitialize focus point
   graphics.focusPoint = {0.0, 0.0, 0.0};

   // Reset game start time
   startTime = std::chrono::high_resolution_clock::now();
   currentPauseDuration = 0.0;
   totalPauseDuration = 0.0;
      
   // Reset sphere   
   physicsEngine.resetSphere(physicsEngine.sphereRigid, physicsEngine.sphereStartPos);
   
   // Update camera view
   graphics.view = glm::lookAt(graphics.eyePosition, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
}

// Returns duration of game
float GameEngine::getGameDuration() {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration_cast< std::chrono::duration<float> >(now-startTime).count();
    
    float gameDuration;
    
    if(isPaused) {
       gameDuration = duration - totalPauseDuration - currentPauseDuration;
    }
    else {
       gameDuration = duration - totalPauseDuration;
    }
    
    // Round to two decimal places
    return floor(gameDuration * 100 + 0.5) / 100;
}

float GameEngine::getPauseDuration() {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration_cast< std::chrono::duration<float> >(now-pauseStartTime).count();

    // Round to two decimal places
    return floor(duration * 100 + 0.5) / 100;
}

// Returns the time delta
float GameEngine::getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}
