#include "PhysicsEngine.h"
#include "GameEngine.h"

// Constructor for physics engine
PhysicsEngine::PhysicsEngine(){
    yGroundPos = 0.2f; // Y position of table ground
    yHiddenPos = -5.0f; // Y position of hiddle 4-player objects
    paddleSize = 2.25f; // Size of paddle in x and z
    puckSize = 2.0f; // Size of puck in x and z
    paddle1Speed = 0.8f;
    inversePaddle2Speed = 10.0f; // Low values = faster paddle, high values = slower paddle
    MAX_VELOCITY = 50.0f; // Max puck velocity
    MIN_VELOCITY = -50.0f;
    
    aiSpeed = 0.5f;
    
    t1 = std::chrono::high_resolution_clock::now(); // Time variable
    
    paddle1StartPos = { -10, yGroundPos, 0 }; // Starting position for paddle 1
    paddle2StartPos = { 10, yGroundPos, 0 }; // Starting position for paddle 2
    paddle3StartPos = { -10, yHiddenPos, 6 }; // Starting position for paddle 3
    paddle4StartPos = { 10, yHiddenPos,  6}; // Starting position for paddle 4
    puckStartPos = { 0, yGroundPos, 0 }; // Starting position for puck
    puck2StartPos = { 0, yGroundPos, 6 }; // Starting position for puck 2

    paddle1Offset = { 0, 0, 0 }; // Used to move paddle 1
    paddle2Offset = { 0, 0, 0 }; // Used to move paddle 2
    paddle3Offset = { 0, 0, 0 }; // Used to move paddle 3
    paddle4Offset = { 0, 0, 0 }; // Used to move paddle 4
    paddle1Bounds = { -41.0f, -2.0f, -21.0f, 21.0f }; // Keeps kinematic paddle 1 in-bounds
    paddle2Bounds = { 2.0f, 41.0f, -21.0f, 21.0f }; // Keeps kinematic paddle 2 in-bounds
    paddle3Bounds = { -41.0f, -2.0f, -21.0f, 21.0f }; // Keeps kinematic paddle 3 in-bounds
    paddle4Bounds = { 2.0f, 41.0f, -21.0f, 21.0f }; // Keeps kinematic paddle 4 in-bounds

    paddle1Rigid = NULL;
    paddle2Rigid = NULL;
    paddle3Rigid = NULL;
    paddle4Rigid = NULL;
    puckRigid    = NULL;
    puck2Rigid   = NULL;
    tableRigid   = NULL;

    // Set collision events
    tableCollidesWith    = COL_PADDLE | COL_PUCK;
    paddleCollidesWith   = COL_TABLE | COL_PADDLE | COL_PUCK;
    puckCollidesWith     = COL_TABLE | COL_PADDLE | COL_PUCK;
}

// Destructor for physics engine
PhysicsEngine::~PhysicsEngine(){
}

// Initializes game physics
void PhysicsEngine::initialize(GameEngine* ptr){
    gameEngine = ptr;

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
    tableRigid   = new btRigidBody(tableRBCI);

    // Add table rigid bodies to dynamic world
    dynamicsWorld->addRigidBody(tableRigid, COL_TABLE, tableCollidesWith);
    
    // Create paddles
    paddle1Rigid = createPaddle(paddle1StartPos);
    paddle2Rigid = createPaddle(paddle2StartPos);
    paddle3Rigid = createPaddle(paddle3StartPos);
    paddle4Rigid = createPaddle(paddle4StartPos);
    
    // Create pucks
    puckRigid = createPuck(puckStartPos);
    puck2Rigid = createPuck(puck2StartPos);
}

// Perform game physics in update
void PhysicsEngine::update(){
    if(!(gameEngine->isPaused)) {
      float dt = getDT();

      // World transform for models
      btTransform paddle1T;
      btTransform paddle2T;
      btTransform paddle3T;
      btTransform paddle4T;
      btTransform puckT;
      btTransform puck2T;

      // Scalars for models
      btScalar paddle1[16];
      btScalar paddle2[16];
      btScalar paddle3[16];
      btScalar paddle4[16];
      btScalar puck[16];
      btScalar puck2[16];
       
      // Step simulation
      dynamicsWorld->stepSimulation(dt,2);

      // Motion states and world transforms
      paddle1Rigid->getMotionState()->getWorldTransform(paddle1T);
      paddle2Rigid->getMotionState()->getWorldTransform(paddle2T);
      paddle3Rigid->getMotionState()->getWorldTransform(paddle3T);
      paddle4Rigid->getMotionState()->getWorldTransform(paddle4T);
      puckRigid->getMotionState()->getWorldTransform(puckT);
      puck2Rigid->getMotionState()->getWorldTransform(puck2T);
       
      // Set models
      paddle1T.getOpenGLMatrix(paddle1);
      paddle2T.getOpenGLMatrix(paddle2);
      paddle3T.getOpenGLMatrix(paddle3);
      paddle4T.getOpenGLMatrix(paddle4);
      puckT.getOpenGLMatrix(puck);
      puck2T.getOpenGLMatrix(puck2);

      gameEngine->models[1] = glm::make_mat4(paddle1);
      gameEngine->models[2] = glm::make_mat4(paddle2);
      gameEngine->models[3] = glm::make_mat4(paddle3);
      gameEngine->models[4] = glm::make_mat4(paddle4);
      gameEngine->models[5] = glm::make_mat4(puck);
      gameEngine->models[6] = glm::make_mat4(puck2);

      // If AI enabled, calculate paddle 1 movement
      if(gameEngine->aiEnabled) {
         // Get x and z coordinates
         float puckX = gameEngine->models[5][3][0];
         float puckZ = gameEngine->models[5][3][2];
         float paddleX = gameEngine->models[1][3][0];
         float paddleZ = gameEngine->models[1][3][2];  
               
         // Calculate puck and paddle distance from goal
         float puckD = sqrt(pow(puckX - paddle1Bounds.x, 2) + pow(puckZ - 0, 2));
         float paddleD = sqrt(pow(paddleX - paddle1Bounds.x, 2) + pow(paddleZ - 0, 2));
               
         // Defense mode if puck on opponent side or puck closer to goal
         if(puckX > 0 || puckD < paddleD) {
            // Get x direction
            if(paddleX < -35.0) {
               paddle1Offset.x = aiSpeed;
            }
            else {
               paddle1Offset.x = -1 * aiSpeed;
            }
            
            // Get z direction
            if(paddleZ < 0) {
               paddle1Offset.z = aiSpeed;
            }
            else {
               paddle1Offset.z = -1 * aiSpeed;
            }            
         }
         
         // Otherwise, attack mode
         else {
            // Get x direction
            if(paddleX < puckX) {
               paddle1Offset.x = aiSpeed;
            }
            else {
               paddle1Offset.x = -1 * aiSpeed;
            }
            
            // Get z direction
            if(paddleZ < puckZ) {
               paddle1Offset.z = aiSpeed;
            }
            else {
               paddle1Offset.z = -1 * aiSpeed;
            }         
         }

      }

      // Check that paddles in 4-player mode aren't stuck together
      
      // Check if paddle 1 and paddle 3 are stuck together
      if(paddlesColliding(btVector3(gameEngine->models[1][3][0], yGroundPos, gameEngine->models[1][3][2]), paddle3Rigid)) {
         btTransform transform;

         // If paddle 1 above paddle 3
         if(gameEngine->models[1][3][2] > gameEngine->models[3][3][2]) {
            // Check that moving paddle 1 won't get stuck in wall         
            if(gameEngine->models[1][3][2] + 1.0f < paddle1Bounds.w) {
               btVector3 pos = {gameEngine->models[1][3][0], yGroundPos, gameEngine->models[1][3][2] + 1.0f};
               
               // Move paddle 1 away from paddle 3
               paddle1Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle1Rigid->getMotionState()->setWorldTransform(transform);            
            }         
            else if(gameEngine->models[3][3][2] - 1.0f > paddle1Bounds.z) {
               btVector3 pos = {gameEngine->models[3][3][0], yGroundPos, gameEngine->models[3][3][2] - 1.0f};
               
               // Move paddle 3 away from paddle 1
               paddle3Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle3Rigid->getMotionState()->setWorldTransform(transform);               
            }
         }
         
         // Otherwise paddle 3 above paddle 1
         else {
            // Check that moving paddle 3 won't get stuck in wall         
            if(gameEngine->models[3][3][2] + 1.0f < paddle1Bounds.w) {
               btVector3 pos = {gameEngine->models[3][3][0], yGroundPos, gameEngine->models[3][3][2] + 1.0f};
               
               // Move paddle 3 away from paddle 1
               paddle3Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle3Rigid->getMotionState()->setWorldTransform(transform);            
            }         
            else if(gameEngine->models[1][3][2] - 1.0f > paddle1Bounds.z) {
               btVector3 pos = {gameEngine->models[1][3][0], yGroundPos, gameEngine->models[1][3][2] - 1.0f};
               
               // Move paddle 1 away from paddle 3
               paddle1Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle1Rigid->getMotionState()->setWorldTransform(transform);               
            }
         }
      }
      // Check if paddle 2 and paddle 4 are stuck
      if(paddlesColliding(btVector3(gameEngine->models[2][3][0], yGroundPos, gameEngine->models[2][3][2]), paddle4Rigid)) {
         btTransform transform;

         // If paddle 2 above paddle 4
         if(gameEngine->models[2][3][2] > gameEngine->models[4][3][2]) {
            // Check that moving paddle 2 won't get stuck in wall         
            if(gameEngine->models[2][3][2] + 1.0f < paddle2Bounds.w) {
               btVector3 pos = {gameEngine->models[2][3][0], yGroundPos, gameEngine->models[2][3][2] + 1.0f};
               
               // Move paddle 2 away from paddle 4
               paddle2Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle2Rigid->getMotionState()->setWorldTransform(transform);            
            }         
            else if(gameEngine->models[4][3][2] - 1.0f > paddle2Bounds.z) {
               btVector3 pos = {gameEngine->models[4][3][0], yGroundPos, gameEngine->models[4][3][2] - 1.0f};
               
               // Move paddle 4 away from paddle 2
               paddle4Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle4Rigid->getMotionState()->setWorldTransform(transform);               
            }
         }
         
         // Otherwise, paddle 4 above paddle 2
         else {
            // Check that moving paddle 4 won't get stuck in wall         
            if(gameEngine->models[4][3][2] + 1.0f < paddle2Bounds.w) {
               btVector3 pos = {gameEngine->models[4][3][0], yGroundPos, gameEngine->models[4][3][2] + 1.0f};
               
               // Move paddle 4 away from paddle 2
               paddle4Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle4Rigid->getMotionState()->setWorldTransform(transform);            
            }         
            else if(gameEngine->models[2][3][2] - 1.0f > paddle2Bounds.z) {
               btVector3 pos = {gameEngine->models[2][3][0], yGroundPos, gameEngine->models[2][3][2] - 1.0f};
               
               // Move paddle 2 away from paddle 4
               paddle2Rigid->getMotionState()->getWorldTransform(transform);
               transform.setOrigin(pos);
               paddle2Rigid->getMotionState()->setWorldTransform(transform);               
            }
         }
      }

      // Check if paddle 1 in bounds
      float paddle1X = gameEngine->models[1][3][0] + paddle1Offset.x;
      float paddle1Z = gameEngine->models[1][3][2] + paddle1Offset.z;  
      if((paddle1X > paddle1Bounds.x && paddle1X < paddle1Bounds.y) && (paddle1Z > paddle1Bounds.z && paddle1Z < paddle1Bounds.w)) {
         // Move if not 4-player mode
         if(!gameEngine->is4PlayerMode) {
            // Moves paddle 1
            paddle1T.setOrigin(btVector3(paddle1X, yGroundPos, paddle1Z));
            paddle1Rigid->getMotionState()->setWorldTransform(paddle1T);
         }

         // Otherwise if 4-player mode, move if paddles not colliding 
         else if(gameEngine->is4PlayerMode && !paddlesColliding(btVector3(paddle1X, yGroundPos, paddle1Z), paddle3Rigid)) {
            // Moves paddle 1
            paddle1T.setOrigin(btVector3(paddle1X, yGroundPos, paddle1Z));
            paddle1Rigid->getMotionState()->setWorldTransform(paddle1T);
         }
      }
       
      // Check if paddle 2 in bounds
      float paddle2X = gameEngine->models[2][3][0] + paddle2Offset.x;
      float paddle2Z = gameEngine->models[2][3][2] + paddle2Offset.z;
      if((paddle2X > paddle2Bounds.x && paddle2X < paddle2Bounds.y) && (paddle2Z > paddle2Bounds.z && paddle2Z < paddle2Bounds.w)) {
         // Move if not 4-player mode
         if(!gameEngine->is4PlayerMode) {
            // Moves paddle 2
            paddle2T.setOrigin(btVector3(paddle2X, yGroundPos, paddle2Z));
            paddle2Rigid->getMotionState()->setWorldTransform(paddle2T);
         }
         
         // Otherwise if 4-player mode, move if paddles not colliding 
         else if(gameEngine->is4PlayerMode && !paddlesColliding(btVector3(paddle2X, yGroundPos, paddle2Z), paddle4Rigid)) {
            // Moves paddle 2
            paddle2T.setOrigin(btVector3(paddle2X, yGroundPos, paddle2Z));
            paddle2Rigid->getMotionState()->setWorldTransform(paddle2T);
         }
      }  

      // Clamp puck speed
      btVector3 velocity = puckRigid->getLinearVelocity();

      // x-axis for puck 1
      if(velocity.getX() > MAX_VELOCITY) {
          velocity.setX(MAX_VELOCITY);
      }
      else if(velocity.getX() < MIN_VELOCITY) {
          velocity.setX(MIN_VELOCITY);
      }
      
      // z-axis for puck 1
      if(velocity.getZ() > MAX_VELOCITY) {  
          velocity.setZ(MAX_VELOCITY);
      }
      else if(velocity.getZ() < MIN_VELOCITY) {  
          velocity.setZ(MIN_VELOCITY);
      }
           
      // Set puck velocities  
      puckRigid->setLinearVelocity(velocity);

      // Check to see if they scored

      // Get puck x and z coordinates
      float puckX = gameEngine->models[5][3][0];
      float puckZ = gameEngine->models[5][3][2];
      int frames = 250;

      // Check to see if puck in paddle 1's goal box
      if((puckX <= paddle1Bounds.x + 0.5) && (puckZ <= 6.4 && puckZ >= -6.4)) {
         // Increase score of player 2
         gameEngine->player2Score++;

         // Play sound effect
         if(gameEngine->pokemon) {
            gameEngine->musicPlayer.pikachu.play();
         }
         else {
            gameEngine->musicPlayer.goal.play();
         }

         // Check if game over
         if(gameEngine->player2Score == 7) {
            gameEngine->gameIsFinished = true;
            gameEngine->p2Win = true;
            gameEngine->resetGame();
            gameEngine->graphics.victoryFrames = frames;
         }

         // Otherwise, reset puck position         
         else {
            resetPuck(puckRigid, puckStartPos);
         }
      }

      // Check to see if puck in paddle 2's goal box 
      else if((puckX >= paddle2Bounds.y - 0.5) && (puckZ <= 6.4 && puckZ >= -6.4)) {
         // Increase score of player 1
         gameEngine->player1Score++;
         
         // Play sound effect
         if(gameEngine->pokemon) {
            gameEngine->musicPlayer.charmander.play();
         }
         else {
            gameEngine->musicPlayer.goal.play();
         }

         // Check if game over
         if(gameEngine->player1Score == 7) {
            gameEngine->gameIsFinished = true;
            gameEngine->p1Win = true;
            gameEngine->resetGame();
            gameEngine->graphics.victoryFrames = frames;
         }

         // Otherwise, reset puck position         
         else {
            resetPuck(puckRigid, puckStartPos);
         }
      }

      // Reset puck 1 if not on table
      if(std::isnan(puckX) || std::isnan(puckZ)) {
         puckRigid = createPuck(puckStartPos);
      }
      else if((puckX < paddle1Bounds.x - 3.0 || puckX > paddle2Bounds.y + 3.0) || 
              (puckZ < paddle1Bounds.z - 3.0 || puckZ > paddle1Bounds.w + 3.0)) {
         resetPuck(puckRigid, puckStartPos);
      }
            
      // Reset movement offset
      paddle1Offset = { 0, 0, 0 };
      paddle2Offset = { 0, 0, 0 };
      
      // If 4-player mode enabled
      if(gameEngine->is4PlayerMode) {
         // Check if paddle 3 in bounds and doesn't collide with paddle 1
         float paddle3X = gameEngine->models[3][3][0] + paddle3Offset.x;
         float paddle3Z = gameEngine->models[3][3][2] + paddle3Offset.z;
         if((paddle3X > paddle3Bounds.x && paddle3X < paddle3Bounds.y) && (paddle3Z > paddle3Bounds.z && paddle3Z < paddle3Bounds.w)
            && !paddlesColliding(btVector3(paddle3X, yGroundPos, paddle3Z), paddle1Rigid)) {
            // Moves paddle 1
            paddle3T.setOrigin(btVector3(paddle3X, yGroundPos, paddle3Z));
            paddle3Rigid->getMotionState()->setWorldTransform(paddle3T);
         }
        
         // Check if paddle 4 in bounds and doesn't collide with paddle 2
         float paddle4X = gameEngine->models[4][3][0] + paddle4Offset.x;
         float paddle4Z = gameEngine->models[4][3][2] + paddle4Offset.z;
         if((paddle4X > paddle4Bounds.x && paddle4X < paddle4Bounds.y) && (paddle4Z > paddle4Bounds.z && paddle4Z < paddle4Bounds.w)
            && !paddlesColliding(btVector3(paddle4X, yGroundPos, paddle4Z), paddle2Rigid)) {
            // Moves paddle 4
            paddle4T.setOrigin(btVector3(paddle4X, yGroundPos, paddle4Z));
            paddle4Rigid->getMotionState()->setWorldTransform(paddle4T);
         }

         // Clamp puck speed
         btVector3 velocity2 = puck2Rigid->getLinearVelocity();

         // x-axis for puck 2
         if(velocity2.getX() > MAX_VELOCITY) {
             velocity2.setX(MAX_VELOCITY);
         }
         else if(velocity2.getX() < MIN_VELOCITY) {
             velocity2.setX(MIN_VELOCITY);
         }

         // z-axis for puck 2
         if(velocity2.getZ() > MAX_VELOCITY) {  
             velocity2.setZ(MAX_VELOCITY);
         }
         else if(velocity2.getZ() < MIN_VELOCITY) {  
             velocity2.setZ(MIN_VELOCITY);
         }
              
         // Set puck velocities  
         puck2Rigid->setLinearVelocity(velocity2);

         // Check to see if they scored

         // Get puck x and z coordinates
         float puck2X = gameEngine->models[6][3][0];
         float puck2Z = gameEngine->models[6][3][2];

         // Check to see if puck 2 in paddle 1's goal box
         if((puck2X <= paddle1Bounds.x + 1.0) && (puck2Z <= 6.4 && puck2Z >= -6.4)) {
            // Increase score of player 2
            gameEngine->player2Score++;

            // Play sound effect
            if(gameEngine->pokemon) {
               gameEngine->musicPlayer.pikachu.play();
            }
            else {
               gameEngine->musicPlayer.goal.play();
            }

            // Check if game over
            if(gameEngine->player2Score == 7) {
               gameEngine->resetGame();
            }

            // Otherwise, reset puck position         
            else {
               resetPuck(puck2Rigid, puck2StartPos);
            }
         }

         // Check to see if puck 2 in paddle 2's goal box 
         else if((puck2X >= paddle2Bounds.y - 1.0) && (puck2Z <= 6.4 && puck2Z >= -6.4)) {
            // Increase score of player 1
            gameEngine->player1Score++;
   
            // Play sound effect
            if(gameEngine->pokemon) {
               gameEngine->musicPlayer.charmander.play();
            }
            else {
               gameEngine->musicPlayer.goal.play();
            }

            // Check if game over
            if(gameEngine->player1Score == 7) {
               gameEngine->resetGame();
            }

            // Otherwise, reset puck position         
            else {
               resetPuck(puck2Rigid, puck2StartPos);
            }
         }
         
         // Reset puck 2 if not on table
         if(std::isnan(puck2X) || std::isnan(puck2Z)) {
            puck2Rigid = createPuck(puck2StartPos);
         }
         else if((puck2X < paddle1Bounds.x - 3.0 || puck2X > paddle2Bounds.y + 3.0) || 
                 (puck2Z < paddle1Bounds.z - 3.0 || puck2Z > paddle1Bounds.w + 3.0)) {
            resetPuck(puck2Rigid, puck2StartPos);
         }
         
         // Reset movement offset
         paddle3Offset = { 0, 0, 0 };
         paddle4Offset = { 0, 0, 0 };
      }
      
      // Otherwise, hide 4-player mode objects
      else {
            paddle3T.setOrigin(paddle3StartPos);
            paddle3Rigid->getMotionState()->setWorldTransform(paddle3T);
            
            paddle4T.setOrigin(paddle4StartPos);
            paddle4Rigid->getMotionState()->setWorldTransform(paddle4T);
            
            resetPuck(puck2Rigid, btVector3(0,yHiddenPos,0));
      }
   }
}

float PhysicsEngine::getDT(){
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

// Reset puck position
void PhysicsEngine::resetPuck(btRigidBody *&rb, btVector3 startPos) {
   // Remove current puck rigid body
   dynamicsWorld->removeRigidBody(rb);
   
   // Remove motion state and rigid body
   delete rb->getMotionState();
   delete rb;

   // Create new puck    
   rb = createPuck(startPos);
}

// Reset paddle positions
void PhysicsEngine::resetPaddles() {
   btTransform transform;

   // Reset paddle 1
   paddle1Rigid->getMotionState()->getWorldTransform(transform);
   transform.setOrigin(paddle1StartPos);         
   paddle1Rigid->getMotionState()->setWorldTransform(transform);

   // Reset paddle 2
   paddle2Rigid->getMotionState()->getWorldTransform(transform);
   transform.setOrigin(paddle2StartPos);
   paddle2Rigid->getMotionState()->setWorldTransform(transform);

   // Reset paddle 3
   paddle3Rigid->getMotionState()->getWorldTransform(transform);
   transform.setOrigin(paddle3StartPos);         
   paddle3Rigid->getMotionState()->setWorldTransform(transform);

   // Reset paddle 4
   paddle4Rigid->getMotionState()->getWorldTransform(transform);
   transform.setOrigin(paddle4StartPos);
   paddle4Rigid->getMotionState()->setWorldTransform(transform);
}

// Create new puck and return rigid body pointer
btRigidBody* PhysicsEngine::createPuck(btVector3 startPos) {
    btScalar mass(1);
    btVector3 inertia(0,0,0);
    
    // Collision shape
    btCollisionShape *puck = new btCylinderShape(btVector3(puckSize,1,puckSize));   

    // Default motion state
    btDefaultMotionState *puckMotion = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), startPos));

    // Calculate local inertia of puck
    puck->calculateLocalInertia(mass,inertia);

    // Create rigid body
    btRigidBody::btRigidBodyConstructionInfo puckRBCI(mass, puckMotion, puck, inertia);
    puckRBCI.m_friction = 0.0;
    puckRBCI.m_restitution = 0.5;
    btRigidBody *rb = new btRigidBody(puckRBCI);
    
    // Disable deactivation
    rb->setActivationState(DISABLE_DEACTIVATION);
        
    // Constrain puck to x-z plane
    rb->setLinearFactor(btVector3(1,0,1));
    rb->setAngularFactor(btVector3(0,0,0));
    
    // Add rigid body to dynamics world
    dynamicsWorld->addRigidBody(rb, COL_PUCK, puckCollidesWith);
    
    // Return rigid body pointer
    return rb;
}

// Create paddle object and return its rigid body
btRigidBody* PhysicsEngine::createPaddle(btVector3 startPos) {
    // Create Bullet collision for objects
    btCollisionShape *paddle = new btCylinderShape(btVector3(paddleSize, 1, paddleSize)); 

    // Default motion state
    btDefaultMotionState *paddleMotion = new btDefaultMotionState(btTransform (btQuaternion (0,0,0,1), startPos));
    
    // Create rigid body construction info
    btRigidBody::btRigidBodyConstructionInfo paddleRBCI(0, paddleMotion, paddle, btVector3(0,0,0));
    
    // Set friction and restitution parameters for rigid body
    paddleRBCI.m_friction = 0.5;
    paddleRBCI.m_restitution = 0.5;  
    
    // Create rigid bodies
    btRigidBody *rb = new btRigidBody(paddleRBCI);
    
    // Set kinematics of paddles
    rb->setCollisionFlags(rb->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    
    // Disable deactivation
    rb->setActivationState(DISABLE_DEACTIVATION);
    
    // Add rigid bodies to dynamic world
    dynamicsWorld->addRigidBody(rb, COL_PADDLE, paddleCollidesWith);
    
    // Return rigid body pointer
    return rb;
}

// Detect collision between two paddles
bool PhysicsEngine::paddlesColliding(btVector3 nextPos, btRigidBody* other) {
    btVector3 otherPos = other->getCenterOfMassPosition();

    // Calculate distance between next position and other rigid body
    float d = sqrt(pow(nextPos.x() - otherPos.x(), 2) + pow(nextPos.z() - otherPos.z(), 2));
    
    // If distance between rigid bodies is less than paddle size, then paddles colliding
    if(d/1.75 < paddleSize) {
        return true;
    }
    
    // Otherwise, paddles not colliding
    return false;
}


