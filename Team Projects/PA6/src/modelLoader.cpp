#include "modelLoader.h"

/**
Constructors for Model class.
*/
Model::Model() 
{

}

Model::Model(const char *path) 
{
   // Load model
   setModel(path);
}

/**
Destructor for Model class.
*/
Model::~Model() 
{

}

/**
Loads object model from specified file. Uses diffuse values found in materials
file for model colors.

@param path File path of the .obj file
@return Vector containing vertice positions and colors
*/
bool Model::setModel(const char *path) 
{
   Assimp::Importer importer;
   const aiScene *myScene = importer.ReadFile(path, aiProcess_Triangulate);
   
   if(myScene == NULL) 
   {
      std::cerr << "[F] FAILED TO READ OBJECT FILE!" << std::endl;
      return false;
   }

   // Iterate through all meshes
   for(unsigned int i=0; i<myScene->mNumMeshes; i++) 
   {
       const aiMesh *mesh = myScene->mMeshes[i];

       if( !(mesh->HasTextureCoords(0)) )
       {
         std::cerr << "[F] MESH DOES NOT CONTAIN TEXTURE COORDINATES" << std::endl;
         return false;
       }


       // Iterate through faces
       for(unsigned int i=0; i<mesh->mNumFaces; i++) 
       {
          const aiFace face = mesh->mFaces[i];
          
          // j<3 because triangulation used - three vertices per face
          for(unsigned int j=0; j<face.mNumIndices; j++) 
          {
             aiVector3D position = mesh->mVertices[face.mIndices[j]];
             aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];

             model.push_back({ {position.x, 
                                position.y, 
                                position.z}, 
                               {uv.x,
                                uv.y} });
          }      
       }   
   }
   
   return true;
}

/**
Returns model (Vertex vector)

@return model
*/
std::vector<Vertex> Model::getModel() const {
   return model;
}

/**
Returns buffer size of entire vector

@return Size of vector
*/
int Model::size() const {
   // Number vertices * number positions & colors * size of GLfloat
   return model.size() * 6 * 4;
}

/**
Returns number of vertices in a model

@return Number of vertices
*/
int Model::numVertices() const {
   return model.size();
}

