#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <iostream> // To display errors
#include <fstream> // Read in vertex and fragment CelestialBodys
#include <chrono>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to CelestialBodys easier
#include <Magick++.h> // Loads in images for celestial body textures

#include "modelLoader.h"

class CelestialBody {
   public:
      // Constructor/Destructor
      CelestialBody();
      CelestialBody(std::string name);
      ~CelestialBody();

      // GLUT Callbacks
      void render(const glm::mat4 vp);
      void update(float dt, glm::mat4 matrix, std::string bodyOfRotation);

      // Resource management
      bool initialize(int width, int height);
      void cleanUp();

      // Member functions
      glm::mat4 getTranslateModel();
      glm::mat4 getModel();
      int getObjIndex();
      std::string getObjName();
      void pause();
      void toggleView();
      void initInfo(int, std::string[]);
      void printText(const char *text, float x, float y);
      void printInfo();
      void drawOrbit(const glm::mat4 vp);

   private:
      // Other members
      std::string objName;
      std::string textName;
      std::string objPath;
      std::string* infoStrings; // Information about celestial body
      int numVertices;
      int numInfoStrings;
      float orbitAngle, rotateAngle;
      float orbitSpeed, rotationSpeed;
      int objIndex; // Index of the celestial body that this is orbitting around
      float scale, distance, actualScale, actualDistance;
      bool isPaused;
      bool isActualView;

      // OpenGL members
      GLuint program; // The GLSL program handle
      GLuint aTexture; // Texture of celestial body
      GLuint vbo_geometry; // VBO handle for our geometry
      GLuint vbo_orbit; // VBO handle for drawing the orbit path
      GLint loc_mvpmat; // Location of the model view projection matrix
      GLint loc_position; // Location of celestial body position
      GLint loc_texture; // Location of celestial body texture

      // Transform matrices
      glm::mat4 model;
      glm::mat4 mvp;
      glm::mat4 translateModel;

      // Helper functions
      bool setSpecifications();
      GLuint buildProgram(const char *vsFilename, const char *fsFilename);
};

#endif
