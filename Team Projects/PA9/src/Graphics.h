#ifndef Graphics_H
#define Graphics_H
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

#define NUM_OBJECTS 7

struct Vertex {
    GLfloat position[3];
    GLfloat uv[2];
};

class GameEngine;
class PhysicsEngine;

class Graphics {
   public:
      // Constructor/Destructor
      Graphics();
      ~Graphics();

      bool initialize(GameEngine* ptr);
      void update();
      void render();
      void reshape(int n_w, int n_h);
      bool modelLoader(std::string fileName, int index);
      std::string readShader(std::string arg);
      void printText( const char * string, float x, float y);
      void victoryScreen();

      friend class GameEngine;
      friend class MusicPlayer;
      friend class PhysicsEngine;

   private:
      GameEngine *gameEngine;

      int w, h; // Window size
      int victoryFrames;
      double cameraRadius; // Distance from focus point (0,0,0)
      double xAxisRadians; // Latitude
      double yAxisRadians; // Longitude
      glm::vec3 eyePosition; // Camera position

      // Shader locations
      GLint loc_position;
      GLint loc_color;
      GLint loc_texture;
      GLint loc_mvpmat;

      // OpenGL global variables
      glm::mat4 view;
      glm::mat4 projection;
      glm::mat4* mvp;
      GLuint program;
      GLuint* vbo_geometry;
      GLuint* aTexture;
      std::vector<Vertex>* geometryModels;
      std::string victoryStrings[8] = {"Player 1 Wins!!","Player 2 Wins!!", "Team 1 Wins!!","Team 2 Wins!!","Team JIGGLYPUFF Wins!!", "Team PICKACHU Wins","JIGGLYPUFF Wins!!","PIKACHU Wins!!"};
};

#endif
