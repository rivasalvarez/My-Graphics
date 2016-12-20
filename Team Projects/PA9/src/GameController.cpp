#include "GameController.h"
#include "GameEngine.h"

// Constructor for game controller
GameController::GameController(){
    speed = 35;    
    lambda = 1;
    delay  = 1;
    t1 = std::chrono::high_resolution_clock::now();
}

// Destructor for game controller
GameController::~GameController(){

}

// Update game movement with controllers
void GameController::update(){
     // X/Y Left Analog
     // Z/R Right Analog
     // U/V Left/Right Triggers

     // Movement
     sf::Joystick::update();
     float player1X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
     float player1Z = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
     float player2X = sf::Joystick::getAxisPosition(1, sf::Joystick::X);
     float player2Z = sf::Joystick::getAxisPosition(1, sf::Joystick::Y);

     float player3X = sf::Joystick::getAxisPosition(0, sf::Joystick::Z);
     float player3Z = sf::Joystick::getAxisPosition(0, sf::Joystick::R);
     float player4X = sf::Joystick::getAxisPosition(1, sf::Joystick::Z);
     float player4Z = sf::Joystick::getAxisPosition(1, sf::Joystick::R);

    // Move player 1
    if(player1X > lambda || player1X < -lambda || player1Z > lambda ||player1Z < -lambda){
       // Move paddle1 along x-axis
       gameEngine->physicsEngine.paddle1Offset.x = (player1X / speed) / gameEngine->physicsEngine.inversePaddle2Speed;    
       
       // Move paddle1 along z-axis
       gameEngine->physicsEngine.paddle1Offset.z = (player1Z / speed) / gameEngine->physicsEngine.inversePaddle2Speed;
    }

    // Move player 2
    if(player2X > lambda || player2X < -lambda || player2Z > lambda || player2Z < -lambda){
       // Move paddle2 along x-axis
       gameEngine->physicsEngine.paddle2Offset.x = (player2X / speed) / gameEngine->physicsEngine.inversePaddle2Speed;    

       // Move paddle2 along z-axis
       gameEngine->physicsEngine.paddle2Offset.z = (player2Z / speed) / gameEngine->physicsEngine.inversePaddle2Speed;
    }

    // Move player 3
    if(player3X > lambda || player3X < -lambda || player3Z > lambda || player3Z < -lambda){
       // Move paddle3 along x-axis
       gameEngine->physicsEngine.paddle3Offset.x = (player3X / speed) / gameEngine->physicsEngine.inversePaddle2Speed;    
       
       // Move paddle3 along z-axis
       gameEngine->physicsEngine.paddle3Offset.z = (player3Z / speed) / gameEngine->physicsEngine.inversePaddle2Speed;
    }

    // Move player 4
    if(player4X > lambda || player4X < -lambda || player4Z > lambda || player4Z < -lambda){
       // Move paddle4 along x-axis
       gameEngine->physicsEngine.paddle4Offset.x = (player4X / speed) / gameEngine->physicsEngine.inversePaddle2Speed;    

       // Move paddle4 along z-axis
       gameEngine->physicsEngine.paddle4Offset.z = (player4Z / speed) / gameEngine->physicsEngine.inversePaddle2Speed;
    }
}

// Check if second PS4 controller is connected
bool GameController::isConnected() {
   bool connected = false;
   
   // Check each joystick for two controllers
   if(sf::Joystick::isConnected(1)) {
         connected = true;
   }

   return connected;
}

bool GameController::initialize(GameEngine* ptr){
    gameEngine = ptr;
    sf::Joystick::update();
    return 0;
}
