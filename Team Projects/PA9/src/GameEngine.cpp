#include "GameEngine.h"
#include "MusicPlayer.h"

// Constructor for game engine
GameEngine::GameEngine(){
    player1Score = 0;
    player2Score = 0;
    isPaused = false;
    isMusicPaused = false;
    is4PlayerMode = false;
    p1Win = false;
    p2Win = false;
    gameIsFinished = false;
    pokemon = false;
    aiEnabled = false;
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
    physicsEngine.initialize(this);
    gameController.initialize(this);

    return true;
}

// Callback function that handles keyboard input
void GameEngine::keyboard(unsigned char key, int x_pos, int y_pos) {
   // If the game is finished, reset booleans to false. NOTE: if a keyboard was pressed, the victory notification is cleared
    if(gameIsFinished) {
       p1Win = p2Win = gameIsFinished = false;
    }
   
    // Handle keyboard input
    switch(key) {
       // Esc, quit program
       case 27:
          exit(0);
          break;

       // Paddle 1 movement
       
       // Move paddle 1 left
       case 'a':
          physicsEngine.paddle1Offset.x = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 1 up
       case 'w':
          physicsEngine.paddle1Offset.z = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 1 down
       case 's':
          physicsEngine.paddle1Offset.z = physicsEngine.paddle1Speed;
          break;

       // Move paddle 1 right
       case 'd':
          physicsEngine.paddle1Offset.x = physicsEngine.paddle1Speed;
          break;

       // Move paddle 1 up-left
       case 'q':
          physicsEngine.paddle1Offset.z = physicsEngine.paddle1Speed * -1;
          physicsEngine.paddle1Offset.x = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 1 up-right
       case 'e':
          physicsEngine.paddle1Offset.z = physicsEngine.paddle1Speed * -1;
          physicsEngine.paddle1Offset.x = physicsEngine.paddle1Speed;
          break;

       // Move paddle 1 down-left
       case 'z':
          physicsEngine.paddle1Offset.z = physicsEngine.paddle1Speed;
          physicsEngine.paddle1Offset.x = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 1 down-right
       case 'c':
          physicsEngine.paddle1Offset.z = physicsEngine.paddle1Speed;
          physicsEngine.paddle1Offset.x = physicsEngine.paddle1Speed;
          break;
         
       // Paddle 2 movement

       // Move paddle 2 left
       case 'j':
          physicsEngine.paddle2Offset.x = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 2 up
       case 'i':
          physicsEngine.paddle2Offset.z = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 2 down
       case 'k':
          physicsEngine.paddle2Offset.z = physicsEngine.paddle1Speed;
          break;

       // Move paddle 2 right
       case 'l':
          physicsEngine.paddle2Offset.x = physicsEngine.paddle1Speed;
          break;

       // Move paddle 2 up-left
       case 'u':
          physicsEngine.paddle2Offset.z = physicsEngine.paddle1Speed * -1;
          physicsEngine.paddle2Offset.x = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 2 up-right
       case 'o':
          physicsEngine.paddle2Offset.z = physicsEngine.paddle1Speed * -1;
          physicsEngine.paddle2Offset.x = physicsEngine.paddle1Speed;
          break;

       // Move paddle 2 down-left
       case 'n':
          physicsEngine.paddle2Offset.z = physicsEngine.paddle1Speed;
          physicsEngine.paddle2Offset.x = physicsEngine.paddle1Speed * -1;
          break;

       // Move paddle 2 down-right
       case ',':
          physicsEngine.paddle2Offset.z = physicsEngine.paddle1Speed;
          physicsEngine.paddle2Offset.x = physicsEngine.paddle1Speed;
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
          
       default:
          break;
    }
}

// Get player 2 score
int GameEngine::getP2Score() {
   return player2Score;
}

// Get player 1 score
int GameEngine::getP1Score() {
   return player1Score;
}

// Special keyboard input to change camera view
void GameEngine::specialKey(int key, int x_pos, int y_pos) {
    double angleChange = M_PI / 16;

    // If the game is finished, reset booleans to false. NOTE: if a keyboard was pressed, the victory notification is cleared
    if(gameIsFinished) {
       p1Win = p2Win = gameIsFinished = false;
    }
   
    // Handle keyboard input
    switch(key) {
       // Move camera view left
       case GLUT_KEY_LEFT:
          // Check if camera is within bounds
          if(graphics.xAxisRadians > (-M_PI / 2)) {
             graphics.xAxisRadians -= angleChange;
          }
          break;

       // Move camera view right
       case GLUT_KEY_RIGHT:
          // Check if camera is within bounds
          if(graphics.xAxisRadians < (M_PI / 2)) {
             graphics.xAxisRadians += angleChange;
          }
          break;

       // Move camera view down
       case GLUT_KEY_DOWN:
          // Check if camera is within bounds
          if(graphics.yAxisRadians > 0) {
             graphics.yAxisRadians = std::max(graphics.yAxisRadians - angleChange, -M_PI / 2);
          }
          break;
       
       // Move camera view up
       case GLUT_KEY_UP:
          // Check if camera is within bounds
          if(graphics.yAxisRadians < (M_PI / 2)) {
             graphics.yAxisRadians = std::min(graphics.yAxisRadians + angleChange, M_PI / 2);
          }
          break;

       default:
          break;
    }
    
    // Calculate eye position
    graphics.eyePosition.x = graphics.cameraRadius * cos(graphics.yAxisRadians) * sin(graphics.xAxisRadians);
    graphics.eyePosition.z = graphics.cameraRadius * cos(graphics.yAxisRadians) * cos(graphics.xAxisRadians);
    graphics.eyePosition.y = graphics.cameraRadius * sin(graphics.yAxisRadians);
}

// Callback function for mouse input (change view, zoom in/out)
void GameEngine::mouse(int button, int state, int x, int y) {
    // If the game is finished, reset booleans to false. NOTE: if a keyboard was pressed, the victory notification is cleared
    if(gameIsFinished) {
       p1Win = p2Win = gameIsFinished = false;
    }

    // Left-click down, hold for paddle movement
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
       mouseX = x;
       mouseY = y;
    }

    // Wheel scroll up to zoom in
    else if(state == GLUT_UP && button == 3 && graphics.cameraRadius > 40.0) {
       graphics.cameraRadius -= 10.0;
    }

    // Wheel scroll down to zoom out
    else if(state == GLUT_UP && button == 4 && graphics.cameraRadius < 200.0) {
       graphics.cameraRadius += 10.0;
    }
   
    // Calculate eye position
    graphics.eyePosition.x = graphics.cameraRadius * cos(graphics.yAxisRadians) * sin(graphics.xAxisRadians);
    graphics.eyePosition.z = graphics.cameraRadius * cos(graphics.yAxisRadians) * cos(graphics.xAxisRadians);
    graphics.eyePosition.y = graphics.cameraRadius * sin(graphics.yAxisRadians);
}

// Callback function that tracks mouse position when button held
void GameEngine::mouseMovement(int x, int y) {
    // Move paddle along x-axis
    physicsEngine.paddle2Offset.x = (x - mouseX) / physicsEngine.inversePaddle2Speed;
    
    // Move paddle along z-axis
    physicsEngine.paddle2Offset.z = (y - mouseY) / physicsEngine.inversePaddle2Speed;

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
         break;
         
      // Quit Game
      case 3:
         exit(0);
         break;

      default:
         break;
    }
    glutPostRedisplay();
}

// Menu commands
void GameEngine::themeMenu(int option) {
    switch(option) {
      // Classic Theme
      case 1:
         pokemon = false;
         musicPlayer.music.openFromFile("Mulan.wav");
         musicPlayer.music.play();
         break;
         
      // Pokemon Theme
      case 2:
         pokemon = true;
         musicPlayer.music.openFromFile("Original.wav");
         musicPlayer.music.play();
         break;

      default:
         break;
    }
    glutPostRedisplay();
}

// Menu commands
void GameEngine::playerMenu(int option) {
    switch(option) {
      // Human/Human
      case 1:
         aiEnabled = false;
         break;
         
      // AI/Human
      case 2:
         // AI/Human mode not enabled for 4 players
         if(!is4PlayerMode) {
            aiEnabled = true;
         }
         else {
            std::cout<<"AI/Human mode not enabled for 4-player mode"<<std::endl;
         }
         break;

      default:
         break;
    }
    glutPostRedisplay();
}

// Menu commands
void GameEngine::gameMenu(int option) {
    switch(option) {
      // 2-player mode
      case 1:
         is4PlayerMode = false;
         break;
         
      // 4-player mode
      case 2:
         // If PS4 controllers plugged in
         if(gameController.isConnected()) {
            is4PlayerMode = true;
            aiEnabled = false;
            resetGame();
         }
         else {
            std::cout<<"Connect two PS4 controllers for 4-player mode"<<std::endl;
         }
         break;

      default:
         break;
    }
    glutPostRedisplay();
}

// Menu commands
void GameEngine::puckMenu(int option) {
    switch(option) {
      // Speed 1
      case 1:
         physicsEngine.MAX_VELOCITY = 10;
         physicsEngine.MIN_VELOCITY = -10;
         break;
         
      // Speed 2
      case 2:
         physicsEngine.MAX_VELOCITY = 50;
         physicsEngine.MIN_VELOCITY = -50;
         break;

      // Speed 3
      case 3:
         physicsEngine.MAX_VELOCITY = 100;
         physicsEngine.MIN_VELOCITY = -100;
         break;
         
      default:
         break;
    }
    glutPostRedisplay();
}

// Reset game
void GameEngine::resetGame() {
   // Reset Player Scores
   player1Score = 0;
   player2Score = 0;

   // Reset positions
   physicsEngine.resetPaddles();
   physicsEngine.resetPuck(physicsEngine.puckRigid, physicsEngine.puckStartPos);
   physicsEngine.resetPuck(physicsEngine.puck2Rigid, physicsEngine.puck2StartPos);
}

// Returns the time delta
float GameEngine::getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}
