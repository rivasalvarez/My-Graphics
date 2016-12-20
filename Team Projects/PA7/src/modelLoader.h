#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <iostream> // To display errors
#include <GL/glew.h> // glew must be included before the main gl libs
#include <assimp/Importer.hpp> // Load models
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>

// Vertex struct used to construct models
struct Vertex {
   GLfloat position[3];
   GLfloat uv[2];
};

class Model {
   public:
      Model();
      Model(std::string path);
      ~Model();

      // Model functions
      bool setModel(std::string path);
      std::vector<Vertex> getModel() const;
      int size() const;
      int numVertices() const;

   private:
      std::vector<Vertex> model;
};

#endif
