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

#define NUM_OBJECTS 7
#define BIT(x) (1<<(x))
enum collisiontypes {
    COL_NOTHING  = 0,      // Collide with nothing
    COL_TABLE    = BIT(1), // Collide with walls
    COL_PADDLE   = BIT(2), // Collide with cylinder
    COL_PUCK     = BIT(3)
};

class GameEngine;
class Graphics;

class PhysicsEngine {
   public:
      // Constructor/Destructor
      PhysicsEngine();
      ~PhysicsEngine();

      void initialize(GameEngine*);
      void update();
      float getDT();
      void resetPuck(btRigidBody *&rb, btVector3 startPos);
      void resetPaddles();
      btRigidBody* createPuck(btVector3 startPos);
      btRigidBody* createPaddle(btVector3 startPos);
      bool paddlesColliding(btVector3 nextPos, btRigidBody* other);
      
      // Friend classes
      friend class GameEngine;
      friend class GameController;
      friend class Graphics;


   private:
      GameEngine *gameEngine;
      Graphics * graphicsEngine;

      // Bullet global variables
      float yGroundPos; // Y position of table ground
      float yHiddenPos; // Y position of hiddle objects for 4-player mode
      btBroadphaseInterface *broadphase;
      btDefaultCollisionConfiguration *collisionConfiguration;
      btCollisionDispatcher *dispatcher;
      btSequentialImpulseConstraintSolver *solver;
      btDiscreteDynamicsWorld *dynamicsWorld;

      // Bullet rigid bodies
      btRigidBody *paddle1Rigid;
      btRigidBody *paddle2Rigid;
      btRigidBody *paddle3Rigid;
      btRigidBody *paddle4Rigid;
      btRigidBody *puckRigid;
      btRigidBody *puck2Rigid;
      btRigidBody *tableRigid;

      int tableCollidesWith;
      int paddleCollidesWith;
      int puckCollidesWith;

      float aiSpeed;

      float paddleSize; // Size of paddle in x and z
      float puckSize; // Size of puck in x and z
      float paddle1Speed;
      float inversePaddle2Speed; // Low values = faster paddle, high values = slower paddle
      float MAX_VELOCITY; // Max puck velocity
      float MIN_VELOCITY;
      
      btVector3 paddle1StartPos; // Starting position for paddle 1
      btVector3 paddle2StartPos; // Starting position for paddle 2
      btVector3 paddle3StartPos; // Starting position for paddle 3
      btVector3 paddle4StartPos; // Starting position for paddle 4
      btVector3 puckStartPos; // Starting position for puck
      btVector3 puck2StartPos; // Starting position for puck

      glm::vec3 paddle1Offset; // Used to move paddle 1
      glm::vec3 paddle2Offset; // Used to move paddle 2
      glm::vec3 paddle3Offset; // Used to move paddle 1
      glm::vec3 paddle4Offset; // Used to move paddle 2
      glm::vec4 paddle1Bounds; // Keeps kinematic paddle 1 in-bounds
      glm::vec4 paddle2Bounds; // Keeps kinematic paddle 2 in-bounds
      glm::vec4 paddle3Bounds; // Keeps kinematic paddle 1 in-bounds
      glm::vec4 paddle4Bounds; // Keeps kinematic paddle 2 in-bounds

      std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
};

#endif
