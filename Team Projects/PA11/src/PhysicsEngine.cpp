#include "PhysicsEngine.h"
#include "GameEngine.h"

// Constructor for physics engine
PhysicsEngine::PhysicsEngine(){
    t1 = std::chrono::high_resolution_clock::now(); // Time variable
    tableRigid   = NULL;

    // Set collision events
    tableCollidesWith = COL_SPHERE;
    sphereCollidesWith  = COL_TABLE;
    sphereSize = 0.75f;
    sphereStartPos = {0.0, 0.0, 0.0};
    sphereStartPos2 = {-39.5, -81, 4.5};
    sphereStartPos3 = {-21, -182, 26.47};

    level1Y = -3.5;
    level2Y = -85.5;
    level3Y = -186.5;
    cp1 = cp2 = cp3 = false;
}

// Destructor for physics engine
PhysicsEngine::~PhysicsEngine(){
}

// Initializes game physics
void PhysicsEngine::initialize(GameEngine* gamePtr, Graphics* graphicsPtr){
    gameEngine = gamePtr;
    graphicsEngine = graphicsPtr;

    // Initialize Bullet variables
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase, solver, collisionConfiguration);

    // Set Bullet parameters
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    // Create Bullet collision for objects  
    btCollisionShape *table = gameEngine->shape[0];

    // Default motion state
    btDefaultMotionState *tableMotion = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), btVector3(0,0,0)));

    // Create rigid body construction info
    btRigidBody::btRigidBodyConstructionInfo tableRBCI(0, tableMotion, table,  btVector3(0,0,0));

    // Set friction and restitution parameters for table
    tableRBCI.m_friction = 0.5;
    tableRBCI.m_restitution = 1.0;

    // Create rigid bodies
    tableRigid = new btRigidBody(tableRBCI);

    // Add table rigid bodies to dynamic world
    dynamicsWorld->addRigidBody(tableRigid, COL_TABLE, tableCollidesWith);

    sphereRigid = createSphere(sphereStartPos);
}

btRigidBody* PhysicsEngine::createSphere(btVector3 startPos) {
    btScalar mass(1);
    btVector3 inertia(0,0,0);
    
    // Collision shape
    btCollisionShape *sphere = new btSphereShape(sphereSize);   

    // Default motion state
    btDefaultMotionState *sphereMotion = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), startPos));

    // Calculate local inertia of puck
    sphere->calculateLocalInertia(mass,inertia);

    // Create rigid body
    btRigidBody::btRigidBodyConstructionInfo sphereRBCI(mass, sphereMotion, sphere, inertia);
    sphereRBCI.m_friction = 0.2;
    sphereRBCI.m_restitution = 0.2;
    btRigidBody *rb = new btRigidBody(sphereRBCI);
    
    // Disable deactivation
    rb->setActivationState(DISABLE_DEACTIVATION);
    
    // Add rigid body to dynamics world
    dynamicsWorld->addRigidBody(rb, COL_SPHERE, sphereCollidesWith);
    
    // Return rigid body pointer
    return rb;
}

// Perform game physics in update
void PhysicsEngine::update(){
    if(!(gameEngine->isPaused)) {
      

      float dt = getDT();
    
      float ballX = gameEngine->models[1][3][0];
      float ballY = gameEngine->models[1][3][1];
      float ballZ = gameEngine->models[1][3][2];

      // Calculate gravity
      float xGravity = -9.81 * graphicsEngine->xAxisRadians;
      float zGravity = -9.81 * (graphicsEngine->yAxisRadians-graphicsEngine->tiltOffset);
      dynamicsWorld->setGravity(btVector3(xGravity, -9.81, zGravity));

      // World transform for models
      btTransform sphereT;

      // Scalars for models
      btScalar sphere[16];
       
      // Step simulation
      dynamicsWorld->stepSimulation(dt,2);

      // Motion states and world transforms
      sphereRigid->getMotionState()->getWorldTransform(sphereT);
       
      // Set models
      sphereT.getOpenGLMatrix(sphere);
      gameEngine->models[1] = glm::make_mat4(sphere);

      // Reached checkpoint 1
      if(ballX < -5 && ballY > -5.0 && !cp1 && !cp2 && !cp3){
         gameEngine->musicPlayer.acheive.play();
         cp1 = true;
      }

      // Reached checkpoint 2
      if(ballX < -20 && ballX > -23 && ballZ > 23 && ballY > -83 && !cp2){
         gameEngine->musicPlayer.acheive.play();
         cp2 = true;
      }

      // Fell on level 1
      if(ballX > -5 && ballY < level1Y && !cp1 && !cp2 && !cp3){
         gameEngine->musicPlayer.fall.play();
         resetSphere(sphereRigid,sphereStartPos);
      }

      // Fell on level 2
      if(ballY < level2Y  && !cp2){
         gameEngine->musicPlayer.fall.play();
         resetSphere(sphereRigid,sphereStartPos2);
      }

      // Fell on level 3
      if(ballY < level3Y  && !cp3){
         gameEngine->musicPlayer.fall.play();
         resetSphere(sphereRigid,sphereStartPos3);
      }

      // Reached end of game
      if(ballX > 50 && ballZ > 95 && !cp3){
         cp3 = true;
         gameEngine->musicPlayer.acheive.play();
         gameEngine->finishTime = gameEngine->getGameDuration();
         gameEngine->resetGame();
         gameEngine->checkHigh();
         gameEngine->sendOutTimes();
      }
   }
}

// Reset ball position
void PhysicsEngine::resetSphere(btRigidBody *&rb, btVector3 startPos) {
   // Remove current puck rigid body
   dynamicsWorld->removeRigidBody(rb);
   
   // Remove motion state and rigid body
   delete rb->getMotionState();
   delete rb;

   // Create new puck    
   rb = createSphere(startPos);
}

float PhysicsEngine::getDT(){
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}
