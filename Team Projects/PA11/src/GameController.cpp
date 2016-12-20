#include "GameController.h"
#include "GameEngine.h"

// Constructor for game controller
GameController::GameController(){
    speed = 10;    
    lambda = 1;
}

// Destructor for game controller
GameController::~GameController(){
}

// Update game movement with controllers
void GameController::update(){
     // Movement
     sf::Joystick::update();
     float playerX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
     float playerZ = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    
    // Tilt board left
    if(playerX > -lambda){
       graphics->xAxisRadians = std::max(graphics->xAxisRadians - (gameEngine->angleChange / speed), gameEngine->leftAngleBound);
    }
    
    // Tilt board right
    if(playerX < lambda){
       graphics->xAxisRadians = std::min(graphics->xAxisRadians + (gameEngine->angleChange / speed), gameEngine->rightAngleBound);
    }
    
    // Tilt board up
    if(playerZ < lambda){
       graphics->yAxisRadians = std::min(graphics->yAxisRadians + (gameEngine->angleChange / speed), gameEngine->topAngleBound);
    }
    
    // Tilt board down
    if(playerZ > -lambda){
       graphics->yAxisRadians = std::max(graphics->yAxisRadians - (gameEngine->angleChange / speed), gameEngine->bottomAngleBound);
    }
}

void GameController::initialize(GameEngine* gameEnginePtr, Graphics* graphicsPtr){
    gameEngine = gameEnginePtr;
    graphics = graphicsPtr;
    sf::Joystick::update();
}
