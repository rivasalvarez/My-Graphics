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

#define NUM_OBJECTS 2
#define NUM_LIGHTS 5

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
      void updateLight();

      friend class GameEngine;
      friend class MusicPlayer;
      friend class PhysicsEngine;
      friend class GameController;
      
   private:
      struct Vertex {
          GLfloat position[3];
          GLfloat normal[3];
          GLfloat uv[2];
      };

      GameEngine *gameEngine;

      int w, h; // Window size
      double cameraRadius; // Distance from focus point (0,0,0)
      double xAxisRadians; // Latitude
      double yAxisRadians; // Longitude
      glm::vec3 eyePosition; // Camera position
      glm::vec3 focusPoint; // Focus point of camera
      double tiltOffset; // Changes camera in y-axis/neutral board tilt position

      // Shader locations
      GLint loc_position;
      GLint loc_color;
      GLint loc_texture;
      GLint loc_norm;
      GLint loc_mvpmat;
      GLint loc_lightPos;
      GLint loc_ambientProd;
      GLint loc_diffuseProd;
      GLint loc_specularProd;
      GLint loc_shininess;
      GLint loc_aAttenParam;
      GLint loc_bAttenParam;
      GLint loc_spotDirection;
      GLint loc_spotCutoff;
      GLint loc_minYLight;

      glm::vec4 lightPosition[NUM_LIGHTS];
      glm::vec3 spotlightDirection[NUM_LIGHTS];
      float spotlightCutoff[NUM_LIGHTS];
      float shininess;
      float aAttenuationParam, // atten = 1.0 / (1.0 + a*dist + b*dist*dist), a = 0 gives more spherical light
            bAttenuationParam; // b = 1.0 / (radius*radius * minLight)
      float minYLight[NUM_LIGHTS]; // Minimum y-coordinate that light can go
      glm::vec4 ambientProduct;
      glm::vec4 diffuseProduct;
      glm::vec4 specularProduct;

      // OpenGL global variables
      glm::mat4 view;
      glm::mat4 projection;
      glm::mat4* mvp;
      GLuint program;
      GLuint* vbo_geometry;
      GLuint* aTexture;
      std::vector<Vertex>* geometryModels;
};

#endif
