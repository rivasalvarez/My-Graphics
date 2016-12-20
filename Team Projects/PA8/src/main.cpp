#define GLM_FORCE_RADIANS 
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <iostream>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier
#include "shaderClass.cpp" 	
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <Magick++.h> 
#include <btBulletDynamicsCommon.h>

#define NUM_OBJECTS 5

struct Vertex {
    GLfloat position[3];
    GLfloat uv[2];
};

// Evil global variables
int w = 640, h = 480;// Window size
float xMoveS = 0;
float zMoveS = 0;
float xMoveC = 0;
float zMoveC = 0;
float speed = 0.5;
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

// Bullet global variables
btBroadphaseInterface *broadphase;
btDefaultCollisionConfiguration *collisionConfiguration;
btCollisionDispatcher *dispatcher;
btSequentialImpulseConstraintSolver *solver;
btDiscreteDynamicsWorld *dynamicsWorld;

btCollisionShape *shape[NUM_OBJECTS];

btRigidBody *sphereRigid = NULL;
btRigidBody *cylinderRigid = NULL;
btRigidBody *cubeRigid = NULL;
btRigidBody *pyramidRigid = NULL;
btRigidBody *tableRigid = NULL;

#define BIT(x) (1<<(x))
enum collisiontypes {
    COL_NOTHING  = 0,      // Collide with nothing
    COL_SPHERE   = BIT(0), // Collide with sphere
    COL_TABLE    = BIT(1), // Collide with walls
    COL_CYLINDER = BIT(2), // Collide with cylinder
    COL_PYRAMID  = BIT(3), // Collide with pyramid
    COL_CUBE     = BIT(4), // Collide with cube
    COL_GROUND   = BIT(5)  // Collide with walls
};

int sphereCollidesWith   = COL_TABLE | COL_CYLINDER | COL_PYRAMID | COL_CUBE | COL_GROUND;
int tableCollidesWith    = COL_SPHERE | COL_CYLINDER | COL_CUBE | COL_PYRAMID;
int cylinderCollidesWith = COL_TABLE | COL_SPHERE | COL_PYRAMID | COL_CUBE | COL_GROUND;
int pyramidCollidesWith  = COL_TABLE |COL_SPHERE | COL_CYLINDER | COL_CUBE | COL_GROUND;
int cubeCollidesWith     = COL_TABLE  | COL_SPHERE | COL_PYRAMID | COL_CYLINDER | COL_GROUND;

// GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
bool modelLoader( std::string fileName, int index );
bool initialize();
void cleanUp();

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
    glutCreateWindow("PA 08");

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
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate projection * view premultiplication
    glm::mat4 vp = projection * view;

    // Render shader objects
    for(int i = 0; i < NUM_OBJECTS; i++) {
      mvp[i] = vp * models[i];

      // Enable the program
      glUseProgram(program);

      // Upload the matrix to the shader object
      glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp[i]));

      // Set up the Vertex Buffer Object so it can be drawn
      glEnableVertexAttribArray(loc_position);
      glEnableVertexAttribArray(loc_texture);
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

      // Draw object
      glDrawArrays(GL_TRIANGLES, 0, geometryModels[i].size() ); // Mode, starting index, count

      // Clean up
      glDisableVertexAttribArray(loc_position);
      glDisableVertexAttribArray(loc_texture);
    }

    // Swap the buffers
    glutSwapBuffers();
}

// Callback function that updates the scene with object transformations and camera view
void update() {
    float dt = getDT();// if you have anything moving, use dt.

    // World transform for models
    btTransform sphereT;
    btTransform pyramidT;
    btTransform cylinderT;
    btTransform cubeT;
    btTransform tableT;

    // Scalars for models
    btScalar sphere[16];
    btScalar cylinder[16];
    btScalar cube[16];
    btScalar table[16];
    btScalar pyramid[16];

    // Step simulation
    dynamicsWorld->stepSimulation(dt,10);

    // Motion states and world transforms
    sphereRigid->getMotionState()->getWorldTransform(sphereT);
    tableRigid->getMotionState()->getWorldTransform(tableT);
    cylinderRigid->getMotionState()->getWorldTransform(cylinderT);
    cubeRigid->getMotionState()->getWorldTransform(cubeT);
    pyramidRigid->getMotionState()->getWorldTransform(pyramidT);
    
    // Set models
    sphereT.getOpenGLMatrix(sphere);
    cylinderT.getOpenGLMatrix(cylinder);
    cubeT.getOpenGLMatrix(cube); 
    tableT.getOpenGLMatrix(table);
    pyramidT.getOpenGLMatrix(pyramid);

    models[0] = glm::make_mat4(table);
    models[1] = glm::make_mat4(sphere);
    models[2] = glm::make_mat4(cylinder);
    models[3] = glm::make_mat4(pyramid);
    models[4] = glm::make_mat4(cube);

    // Move sphere and cylinder
    sphereRigid->translate( btVector3 (xMoveS,0,zMoveS) );
    cylinderRigid->translate( btVector3 (xMoveC,0,zMoveC) );

    // Check if sphere rigid is active
    if(!sphereRigid->isActive()) {
       sphereRigid->activate();
    }
    
    // Check if cylinder rigid is active
    if(!cylinderRigid->isActive()) {
       cylinderRigid->activate();
    }

    // Reset movemenet
    xMoveS = zMoveS = xMoveC = zMoveC = 0;
    
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
                                   1000.0f); // Far plane
}

// Initialize shader objects and view/projection matrices
bool initialize() {
    // Initialize Bullet variables
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    // Set Bullet parameters
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    btScalar mass(1);
    btVector3 inertia(0,0,0);
    bool result = true;
   
    // Set object and texture names
    std::string objNames[NUM_OBJECTS] = {"table.obj","sphere.obj","cylinder.obj","pyramid.obj","cube.obj"};
    std::string textNames[NUM_OBJECTS] = {"blue.jpg","cat.jpg","pink.jpg","yellow.jpg","green.jpg"};

    // Create shaders
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Shader Sources   
    ShaderSrcLoader vertexShader;
    ShaderSrcLoader fragmentShader;
    std::string vertex = vertexShader.readShader("shader.vs");
    std::string fragment = fragmentShader.readShader("shader.fs");
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
                    const_cast<const char*>("v_uv"));
    if(loc_texture == -1) {
        std::cerr << "[F] V_UV NOT FOUND" << std::endl;
        return false;
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
       glGenBuffers(1, &vbo_geometry[i]);
       glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry[i]);
       glBufferData(GL_ARRAY_BUFFER, geometryModels[i].size() * 6 * 4, &geometryModels[i][0], GL_STATIC_DRAW);//

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

    // Create Bullet collision for objects
    btCollisionShape *sphere   = new btSphereShape(1.0f);
    btCollisionShape *cube     = new btBoxShape(btVector3(1,1,1));
    btCollisionShape *cylinder = new btSphereShape(1.0f);
    btCollisionShape *pyramid  = new btBoxShape(btVector3(1,1,1));
    btCollisionShape *table    = shape[0];

    // Default motion state
    btDefaultMotionState *sphereMotion   = NULL;
    btDefaultMotionState *cubeMotion     = NULL;
    btDefaultMotionState *cylinderMotion = NULL;
    btDefaultMotionState *pyramidMotion  = NULL;
    btDefaultMotionState *tableMotion    = NULL;

    sphereMotion   = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), btVector3(4,10,0)));
    cubeMotion     = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), btVector3(0,6.35,0)));
    cylinderMotion = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), btVector3(4,10,0)));
    pyramidMotion  = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), btVector3(4,10,0)));
    tableMotion    = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), btVector3(4,5,0)));

    // Calculate local inertia
    sphere->calculateLocalInertia(mass,inertia);
    cube->calculateLocalInertia(mass,inertia);
    cylinder->calculateLocalInertia(mass,inertia);
    pyramid->calculateLocalInertia(mass,inertia);
    table->calculateLocalInertia(mass,inertia);

    // Create rigid bodies
    btRigidBody::btRigidBodyConstructionInfo sphereRBCI(mass, sphereMotion, sphere, inertia);
    btRigidBody::btRigidBodyConstructionInfo cubeRBCI(0, cubeMotion, cube, btVector3( 0, 0, 0 ));
    btRigidBody::btRigidBodyConstructionInfo cylinderRBCI(mass, cylinderMotion, cylinder, inertia);
    btRigidBody::btRigidBodyConstructionInfo pyramidRBCI(mass, pyramidMotion, pyramid, inertia);
    btRigidBody::btRigidBodyConstructionInfo tableRBCI(0, tableMotion, table,  btVector3( 0, 0, 0 ));

    sphereRigid   = new btRigidBody(sphereRBCI);
    cubeRigid     = new btRigidBody(cubeRBCI);
    cylinderRigid = new btRigidBody(cylinderRBCI);
    pyramidRigid  = new btRigidBody(pyramidRBCI);
    tableRigid    = new btRigidBody(tableRBCI);

    // Set cube and table to status/kinematic objects
    cubeRigid->setCollisionFlags(cubeRigid->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    cubeRigid->setActivationState(DISABLE_DEACTIVATION);

    tableRigid->setCollisionFlags(tableRigid->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    tableRigid->setActivationState(DISABLE_DEACTIVATION);

    // Add rigid bodies to dynamic world
    dynamicsWorld->addRigidBody(sphereRigid,   COL_SPHERE,   sphereCollidesWith);
    dynamicsWorld->addRigidBody(cubeRigid,     COL_CUBE,     cubeCollidesWith);
    dynamicsWorld->addRigidBody(cylinderRigid, COL_CYLINDER, cylinderCollidesWith);
    dynamicsWorld->addRigidBody(pyramidRigid,  COL_PYRAMID,  pyramidCollidesWith);
    dynamicsWorld->addRigidBody(tableRigid,    COL_TABLE,    tableCollidesWith);

    // Initialize the view and projection matrices
    view = glm::lookAt( glm::vec3(0.0, 40.0, -40.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, // FOV
                                   float(w)/float(h),
                                   0.01f, // Near plane
                                   1000.0f); // Far plane

   return result;
}


bool modelLoader( std::string fileName, int index )
{
   Assimp::Importer importer;
   const aiScene *myScene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate);
   btVector3 triArray[3];
   btTriangleMesh *objTriMesh = new btTriangleMesh();
   
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


       // Iterate through faces
       for(unsigned int i=0; i<mesh->mNumFaces; i++)
       {
          const aiFace face = mesh->mFaces[i];

          // j<3 because triangulation used - three vertices per face
          for(unsigned int j=0; j<face.mNumIndices; j++)
          {
             aiVector3D position = mesh->mVertices[face.mIndices[j]];
             aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];
             triArray[j] = btVector3(position.x, position.y, position.z);
             
             geometryModels[index].push_back({ {position.x,
                                         position.y,
                                         position.z},
                                         {uv.x,
                                          uv.y} });
          }
          
          // Set object triangle mesh
          objTriMesh->addTriangle(triArray[0], triArray[1], triArray[2]);
       }
   }
   
   // Store triangle mesh shape
   shape[index] = new btBvhTriangleMeshShape(objTriMesh, true);
   
   return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry[0]);
    glDeleteBuffers(1, &vbo_geometry[1]);
    glDeleteBuffers(1, &vbo_geometry[2]);
    glDeleteBuffers(1, &vbo_geometry[3]);
    
    delete broadphase;
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;
    delete dynamicsWorld;

    broadphase = NULL;
    collisionConfiguration = NULL;
    dispatcher = NULL;
    solver = NULL;
    dynamicsWorld = NULL;
}


// Callback function that handles keyboard input
void keyboard(unsigned char key, int x_pos, int y_pos) {
    // Handle keyboard input
    switch(key) {
       // Esc, quit program
       case 27:
          exit(0);
          break;

       // Move sphere left
       case 'a':
          xMoveS = speed;
          break;

       // Move sphere up
       case 'w':
          zMoveS = speed;
          break;

       // Move sphere down
       case 's':
          zMoveS = -speed;
          break;

       // Move sphere right
       case 'd':
          xMoveS = -speed;
          break;

       // Move cylinder left
       case 'j':
          xMoveC = speed;
          break;

       // Move cylinder up
       case 'i':
          zMoveC = speed;
          break;

       // Move cylinder down
       case 'k':
          zMoveC = -speed;
          break;

       // Move cylinder right
       case 'l':
          xMoveC = -speed;
          break;

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
