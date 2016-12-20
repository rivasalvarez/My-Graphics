#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <iostream> // To display errors

#include <GL/glew.h> // glew must be included before the main gl libs
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>

//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex {
   GLfloat position[3];
   GLfloat color[3];
};

class Model {
   public:
      Model();   
      Model(const char *path);
      ~Model();
      
      // Model functions
      bool setModel(const char *path);
      std::vector<Vertex> getModel() const;
      int size() const;
      int numVertices() const;
      
   private:
      std::vector<Vertex> model;
};

#endif
