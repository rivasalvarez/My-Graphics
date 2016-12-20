#ifndef GameController_H
#define GameController_H

#include <chrono>
#include <iostream>
#include <SFML/Window.hpp>

class GameEngine;
class Graphics;

class GameController {
   public:
      // Constructor/Destructor
      GameController();
      ~GameController();
      void update();
      void initialize(GameEngine* gameEnginePtr, Graphics* graphicsPtr);

   private:
      GameEngine *gameEngine;
      Graphics *graphics;
      float speed;
      float lambda;
};

#endif
