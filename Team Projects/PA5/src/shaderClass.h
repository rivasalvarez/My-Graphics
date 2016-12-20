#ifndef SHADERCLASS_H
#define SHADERCLASS_H

#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <iostream> // To display errors
#include <fstream> // Read in vertex and fragment shaders
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

#include "modelLoader.h"

class Shader {
   public:
      // Constructor/Destructor
      Shader();
      ~Shader();

      // GLUT Callbacks
      void render();
      void update();
      void reshape(int width, int height);

      // Resource management
      bool initialize(const char *path, int width, int height);
      void cleanUp();
      
   private:
      // OpenGL members
      GLuint program;// The GLSL program handle
      GLuint vbo_geometry;// VBO handle for our geometry
      GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader
      GLint loc_position, loc_color;// Attribute locations

      // Transform matrices
      glm::mat4 model;//obj->world each object should have its own model matrix
      glm::mat4 view;//world->eye
      glm::mat4 projection;//eye->clip
      glm::mat4 mvp;//premultiplied modelviewprojection
      
      // Other members
      int numVertices;
      float orbitAngle, rotateAngle;
      std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

      // Helper functions
      float getDT();
      GLuint buildProgram(const char *vsFilename, const char *fsFilename);      
};

#endif
