#ifndef GameController_H
#define GameController_H

#include <chrono>
#include <iostream>
#include <SFML/Window.hpp>

class GameEngine;

class GameController {
   public:
      // Constructor/Destructor
      GameController();
      ~GameController();
      void update();
      bool initialize(GameEngine*);
      bool isConnected();

   private:
      GameEngine *gameEngine;
      float delay;
      float speed;
      float lambda;
      std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
};

#endif
