#ifndef GameEngine_H
#define GameEngine_H
#define GLM_FORCE_RADIANS 
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <iostream>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <Magick++.h> 
#include <btBulletDynamicsCommon.h>
#include <sstream> 
#include <fstream>

#include "MusicPlayer.h"
#include "PhysicsEngine.h"
#include "Graphics.h"
#include "GameController.h"

#define NUM_OBJECTS 7

class MusicPlayer;
class PysicsEngine;
class GameController;
class Graphics;

class GameEngine {
   public:
      // Constructor/Destructor
      GameEngine();
      ~GameEngine();

      void render();
      bool initialize();
      void update();
      void reshape(int nw, int nh);
      void menu(int option);
      void themeMenu(int option);
      void playerMenu(int option);
      void gameMenu(int option);
      void puckMenu(int option);
      void keyboard(unsigned char key, int x_pos, int y_pos);
      void specialKey(int key, int x_pos, int y_pos);
      void mouse(int button, int state, int x, int y);
      void mouseMovement(int x, int y);
      void resetGame();
      int getP1Score();
      int getP2Score();

      // Time stuff
      float getDT();

      // Friend classes
      friend class MusicPlayer;
      friend class PhysicsEngine;
      friend class Graphics;
      friend class GameController;

   private:
      // Friend class members
      Graphics graphics;
      PhysicsEngine physicsEngine;
      MusicPlayer musicPlayer;
      GameController gameController;

      // Game Engine members
      int player1Score;
      int player2Score;
      int mouseX;
      int mouseY;
      bool isPaused;
      bool isMusicPaused;
      bool is4PlayerMode;
      bool p1Win;
      bool p2Win;
      bool gameIsFinished;
      bool pokemon;
      bool aiEnabled;
      
      // Bullet and openGL members
      glm::mat4* models;
      btCollisionShape** shape; // Triangle mesh rigid body
      std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
   
};
#endif
