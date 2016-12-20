#ifndef PhysicsEngine_H
#define PhysicsEngine_H

#define GLM_FORCE_RADIANS 
#include <vector>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <sstream> 
#include <fstream>
#include <cmath>

#define NUM_OBJECTS 2
#define BIT(x) (1<<(x))
enum collisiontypes {
    COL_NOTHING  = 0,      // Collide with nothing
    COL_TABLE    = 1,      // Collide with table
    COL_SPHERE     = 2       // Collide with ball
};

class GameEngine;
class Graphics;

class PhysicsEngine {
   public:
      // Constructor/Destructor
      PhysicsEngine();
      ~PhysicsEngine();

      void initialize(GameEngine*, Graphics*);
      void update();
      float getDT();
      btRigidBody* createSphere(btVector3 startPos);
      void resetSphere(btRigidBody *&rb, btVector3 startPos);
      
      // Friend classes
      friend class GameEngine;
      friend class GameController;
      friend class Graphics;

   private:
      GameEngine *gameEngine;
      Graphics *graphicsEngine;
      double sphereSize;
      int tableCollidesWith, sphereCollidesWith;
      int level1Y, level2Y, level3Y;
      bool cp1, cp2, cp3;
      btVector3 sphereStartPos,sphereStartPos2,sphereStartPos3;

      // Bullet global variables
      btBroadphaseInterface *broadphase;
      btDefaultCollisionConfiguration *collisionConfiguration;
      btCollisionDispatcher *dispatcher;
      btSequentialImpulseConstraintSolver *solver;
      btDiscreteDynamicsWorld *dynamicsWorld;

      // Bullet rigid bodies
      btRigidBody *tableRigid;
      btRigidBody *sphereRigid;

      std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
};

#endif
