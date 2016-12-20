#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <Magick++.h>

#define NUM_OBJECTS 1

struct Vertex {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
};

// Evil global variables
int w = 640, h = 480;// Window size
glm::mat4 view;
glm::mat4 projection;
glm::mat4 mvp[NUM_OBJECTS];
glm::mat4 models[NUM_OBJECTS];
GLuint program;
GLuint vbo_geometry[NUM_OBJECTS];
GLuint aTexture[NUM_OBJECTS];
std::vector<Vertex> geometryModels[NUM_OBJECTS];

// Shader locations
GLint loc_position;
GLint loc_color;
GLint loc_texture;
GLint loc_mvpmat;
GLint loc_normal;
GLint loc_lightPos;
GLint loc_ambientProd;
GLint loc_diffuseProd;
GLint loc_specularProd;
GLint loc_shininess;

glm::vec4 lightPosition;
float shininess;

glm::vec4 ambientProduct;
glm::vec4 diffuseProduct;
glm::vec4 specularProduct;

// Lighting variables
bool ambientOn = false;
bool distanceOn = false;


// GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
bool modelLoader( std::string fileName, int index );
bool initialize();
void cleanUp();
std::string readShader(std::string arg);

// Time stuff
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

// Main
int main(int argc, char **argv) {
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

    // Create window
    glutInitWindowSize(w, h);
    glutCreateWindow("PA 10");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if(status != GLEW_OK) {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";

        return -1;
    }

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();

    // Start glut main loop if successfully initialized
    if(init) {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    return 0;
}

// Implementations

// Callback function that renders the scene and shader objects
void render() {
    // Clear the screen
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate projection * view premultiplication
    glm::mat4 vp = projection * view;

    // Light stuff
    lightPosition = glm::vec4(10.0, 10.0, 20.0, 0.0);
    ambientProduct = glm::vec4(0.5, 0.5, 0.5, 0.0);
    diffuseProduct = glm::vec4(0.5, 0.5, 0.5, 0.0);
    specularProduct = glm::vec4(0.5, 0.5, 0.5, 0.0);
    shininess = 0.5;
    
    // Render shader objects
    for(int i = 0; i < NUM_OBJECTS; i++) {
      mvp[i] = vp * models[i];

      // Enable the program
      glUseProgram(program);

      // Upload the matrix to the shader object
      glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp[i]));

     // Upload light data
    glUniform4fv(loc_lightPos, 1, glm::value_ptr(lightPosition));
    glUniform4fv(loc_ambientProd, 1, glm::value_ptr(ambientProduct));
    glUniform4fv(loc_diffuseProd, 1, glm::value_ptr(diffuseProduct));
    glUniform4fv(loc_specularProd, 1, glm::value_ptr(specularProduct));
    glUniform1f(loc_shininess, shininess);

      // Set up the Vertex Buffer Object so it can be drawn
      glEnableVertexAttribArray(loc_position);
      glEnableVertexAttribArray(loc_texture);
      glEnableVertexAttribArray(loc_normal);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry[i]);

      // Set pointers into the vbo for each of the attributes

      // Position attribute
      glVertexAttribPointer( loc_position, // Location of attribute
                             3, // Number of elements
                             GL_FLOAT, // Type
                             GL_FALSE, // Normalized
                             sizeof(Vertex), // Stride
                             (void*)0); // Offset
                             
      // Texture attribute
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, aTexture[i]);
      glVertexAttribPointer( loc_texture,
                             3,
                             GL_FLOAT,
                             GL_FALSE,
                             sizeof(Vertex),
                             (void*)offsetof(Vertex,uv));

       //normal attribute
       glVertexAttribPointer(loc_normal, 
	   		     3,
	   		     GL_FLOAT,
	   		     GL_FALSE,
	   		     sizeof(Vertex),
	   	              0);
		
  
      // Draw object
      glDrawArrays(GL_TRIANGLES, 0, geometryModels[i].size() ); // Mode, starting index, count

      // Ambient Light
      if(ambientOn) {
         glUniform4f(glGetUniformLocation(program, "ambientLight"), 1.0, 1.0, 1.0, 1);
      }
      else {
         glUniform4f(glGetUniformLocation(program, "ambientLight"), 0.5, 0.5, 0.5, 1);
      }

      // Clean up
      glDisableVertexAttribArray(loc_position);
      glDisableVertexAttribArray(loc_texture);
    }

    // Swap the buffers
    glutSwapBuffers();
}

// Callback function that updates the scene with object transformations and camera view
void update() {
    //float dt = getDT();// if you have anything moving, use dt.

    // Call the display callback
    glutPostRedisplay();
}

// Reshape objects based on new window size
void reshape(int n_w, int n_h) {
    w = n_w;
    h = n_h;

    //Change the viewport to be correct
    glViewport( 0, 0, w, h);

    // Update the projection matrices for shader objects
    projection = glm::perspective( 45.0f, // FOV
                                   float(w)/float(h),
                                   0.01f, // Near plane
                                   100.0f); // Far plane
}

// Initialize shader objects and view/projection matrices
bool initialize() {
    // Set object and texture names
    std::string objNames[NUM_OBJECTS] = {"sphere.obj"};
    std::string textNames[NUM_OBJECTS] = {"cat.jpg"};

    // Create shaders
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Shader Sources
    std::string vertex = readShader("shader.vs");
    std::string fragment = readShader("shader.fs");
    const char *vs = vertex.c_str();
    const char *fs= fragment.c_str();

    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);

    // Check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status) {
        std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
        return false;
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);

    // Check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status) {
        std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
        return false;
    }

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Check if everything linked okay
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status) {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_texture = glGetAttribLocation(program,
                    const_cast<const char*>("v_texture"));
    if(loc_texture == -1) {
        std::cerr << "[F] V_UV NOT FOUND" << std::endl;
        return false;
    }

    loc_normal = glGetAttribLocation(program, 

                    const_cast<const char*>("v_normal"));
     if(loc_normal == -1) {
         std::cerr << "[F] V_NORM NOT FOUND" << std::endl;
     }
 
    loc_lightPos = glGetUniformLocation(program, 
                    const_cast<const char*>("lightPosition"));
    if(loc_lightPos == -1) {
        std::cerr << "[F] LIGHTPOS NOT FOUND" << std::endl;
    }
    
    loc_ambientProd = glGetUniformLocation(program, 
                    const_cast<const char*>("ambientProduct"));
    if(loc_ambientProd == -1) {
        std::cerr << "[F] AMBIENTPRODUCT NOT FOUND" << std::endl;
    }
    
    loc_diffuseProd = glGetUniformLocation(program, 
                    const_cast<const char*>("diffuseProduct"));
    if(loc_diffuseProd == -1) {
       std::cerr << "[F] DIFFUSEPRODUCT NOT FOUND" << std::endl;
    }
    
   loc_specularProd = glGetUniformLocation(program, 
                   const_cast<const char*>("specularProduct"));
    if(loc_specularProd == -1) {
        std::cerr << "[F] SPECULARPRODUCT NOT FOUND" << std::endl;
    }

    loc_shininess = glGetUniformLocation(program, 
                    const_cast<const char*>("shininess"));
    if(loc_shininess == -1) {
        std::cerr << "[F] SHININESS NOT FOUND" << std::endl;
   }
   

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }

    // Initialize shader objects
    for(int i=0; i < NUM_OBJECTS; i++) {
       modelLoader(objNames[i],i);

       // Create a Vertex Buffer object to store this vertex info on the GPU
       // Number vertices * number positions, normals, colors * size of GLfloat
       glGenBuffers(1, &vbo_geometry[i]);
       glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry[i]);
       glBufferData(GL_ARRAY_BUFFER, geometryModels[i].size() * 8 * 4, &geometryModels[i][0], GL_STATIC_DRAW);

       // Apply texture
       Magick::InitializeMagick(textNames[i].c_str());
       Magick::Image image;
       image.read(textNames[i].c_str());
       Magick::Blob blob;
       image.write(&blob,"RGBA");

       glGenTextures(1, &aTexture[i]);
       glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_2D, aTexture[i]);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.columns(), image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
       glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Initialize the view and projection matrices
    view = glm::lookAt( glm::vec3(0.0, 40.0, -40.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, // FOV
                                   float(w)/float(h),
                                   0.01f, // Near plane
                                   100.0f); // Far plane

   return true;
}


bool modelLoader( std::string fileName, int index )
{
   Assimp::Importer importer;
   const aiScene *myScene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate);

   if(myScene == NULL) {
      std::cerr << "[F] FAILED TO READ OBJECT FILE!" << std::endl;
      return false;
   }

   // Iterate through all meshes
   for(unsigned int i=0; i<myScene->mNumMeshes; i++)
   {
       const aiMesh *mesh = myScene->mMeshes[i];

       // Check if texture exists
       if( !(mesh->HasTextureCoords(0)) ) {
         std::cerr << "[F] MESH DOES NOT CONTAIN TEXTURE COORDINATES" << std::endl;
         return false;
       }

       if( !(mesh->HasNormals()) ) {
         std::cerr << "[F] MESH DOES NOT CONTAIN NORMALS" << std::endl;
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
             aiVector3D norm = mesh->mNormals[face.mIndices[j]];
             
             geometryModels[index].push_back({ {position.x,
                                         position.y,
                                         position.z},
                                         {norm.x,
                                          norm.y,
                                          norm.z},
                                         {uv.x,
                                          uv.y} });
          }
       }
   }

   return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry[0]);
}


// Callback function that handles keyboard input
void keyboard(unsigned char key, int x_pos, int y_pos) {
    // Handle keyboard input
    switch(key) {
       // Esc, quit program
       case 27:
          exit(0);
          break;

       // Toggle ambient light
       case 'a':
          ambientOn = !ambientOn;

       default:
          break;
    }
}

// Returns the time delta
float getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

std::string readShader(std::string arg){
    std::string shaderSrc;
    std::ifstream fin;
    std::string temp;

    fin.open(arg.c_str());

    while(!fin.eof()) {
       getline(fin,temp);

       shaderSrc.append(temp + '\n');
    }

    fin.close();
    return shaderSrc;
}
