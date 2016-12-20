#include "Graphics.h"
#include "GameEngine.h"

// Constructor for graphics class
Graphics::Graphics(){
    w = 640; 
    h = 480; // Window size
    victoryFrames = 0;
}

// Destructor for graphics class
Graphics::~Graphics(){
}

// Initialize graphics class
bool Graphics::initialize(GameEngine* ptr){
    gameEngine = ptr; 
    cameraRadius = 75.0; // Distance from focus point (0,0,0)
    xAxisRadians = 0; // Latitude
    yAxisRadians = M_PI / 4; // Longitude

    mvp = new glm::mat4[NUM_OBJECTS];
    gameEngine->models = new glm::mat4[NUM_OBJECTS];
    vbo_geometry = new GLuint[NUM_OBJECTS];
    aTexture = new GLuint[NUM_OBJECTS];
    geometryModels = new std::vector<Vertex>[NUM_OBJECTS];
    gameEngine->shape = new btCollisionShape*[NUM_OBJECTS];

    // Set object and texture names
    std::string objNames[NUM_OBJECTS] = {"table.obj","paddle.obj","paddle.obj","paddle.obj","paddle.obj","puck.obj","puck.obj"};
    std::string textNames[5] = {"battle1.png","battle2.png","red.jpg","lightning.jpg","fire.jpg"};

    // Create shaders
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Shader Sources   
    std::string vertex = readShader("shader.vs");
    std::string fragment = readShader("shader.fs");
    const char *vs = vertex.c_str();
    const char *fs= fragment.c_str();

    // Compile the shaders
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

    // Link fragment shader and vertex shader objects into a program
    // This program is what is run on the GPU
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

    // Set locations of attributes and uniforms
    
    // Position
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    // Texture
    loc_texture = glGetAttribLocation(program,
                    const_cast<const char*>("v_uv"));
    if(loc_texture == -1) {
        std::cerr << "[F] V_UV NOT FOUND" << std::endl;
        return false;
    }

    // MVP matrix
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
    }
    for(int i = 0; i < 5; i++){
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
    // Calculate eye position
    eyePosition.x = cameraRadius * cos(yAxisRadians) * sin(xAxisRadians);
    eyePosition.z = cameraRadius * cos(yAxisRadians) * cos(xAxisRadians);
    eyePosition.y = cameraRadius * sin(yAxisRadians);

    // Initialize view matrix
    view = glm::lookAt( eyePosition, //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    // Initialize projection matrix
    projection = glm::perspective( 45.0f, // FOV
                                   float(w)/float(h),
                                   0.01f, // Near plane
                                   1000.0f); // Far plane

   // Enable depth testing
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   return true;
}

void Graphics::victoryScreen(){
    //Player 1
    if(!gameEngine->is4PlayerMode && !gameEngine->pokemon && gameEngine->p1Win) {
       printText(victoryStrings[0].c_str() , -.1, .5);
    }
    
    //Player 2
    else if(!gameEngine->is4PlayerMode && !gameEngine->pokemon  && gameEngine->p2Win) {
       printText(victoryStrings[1].c_str() , -.1, .5);
    }
    
    //Team 1
    else if(gameEngine->is4PlayerMode && !gameEngine->pokemon  && gameEngine->p1Win) {
       printText(victoryStrings[2].c_str() , -.1, .5);
    }
    
    //Team 2
    else if(gameEngine->is4PlayerMode && !gameEngine->pokemon  && gameEngine->p2Win) {
       printText(victoryStrings[3].c_str() , -.1, .5);
    }
    
    //Team Jiggly
    else if(gameEngine->is4PlayerMode && gameEngine->pokemon  && gameEngine->p1Win) {
       printText(victoryStrings[4].c_str() , -.1, .5);
    }
    
    //Team Pika
    else if(gameEngine->is4PlayerMode && gameEngine->pokemon  && gameEngine->p2Win) {
       printText(victoryStrings[5].c_str() , -.1, .5);
    }
    
    //Jiggly
    else if(!gameEngine->is4PlayerMode && gameEngine->pokemon  && gameEngine->p1Win) {
       printText(victoryStrings[6].c_str() , -.1, .5);
    }
    
    //Pika
    else if(!gameEngine->is4PlayerMode && gameEngine->pokemon  && gameEngine->p2Win) {
       printText(victoryStrings[7].c_str() , -.1, .5);
    }
}

// Callback function that updates camera view
void Graphics::update(){
    // Update camera view
    view = glm::lookAt(eyePosition, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    
    // Call the display callback
    glutPostRedisplay();
}

// Callback function that renders scores and objects
void Graphics::render(){
    char * p1ScoreStr = new char[80];
    char * p2ScoreStr = new char[80];

    // Clear the screen
    glClearColor(.5, .5, .5, .5);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Convert scores to string to be printed
    std::ostringstream  converter;
    std::ostringstream  converter2;
    converter << gameEngine->player1Score;
    std::string p1Score = converter.str();

    converter2 << gameEngine->player2Score;
    std::string p2Score = converter2.str();
   
    // Set score text
    if(gameEngine->is4PlayerMode) {
       strcpy(p1ScoreStr, "Team 1 Score: ");
       strcpy(p2ScoreStr, "Team 2 Score: "); 
    }
    
    else {
       strcpy(p1ScoreStr, "Player 1 Score: ");
       strcpy(p2ScoreStr, "Player 2 Score: ");    
    }

    strcat(p1ScoreStr, p1Score.c_str());
    strcat(p2ScoreStr, p2Score.c_str());

    // Print scores
    printText(p1ScoreStr, -.50, .80);
    printText(p2ScoreStr, .30, .80);

    // Check to see if there is a victory
    if( gameEngine->gameIsFinished  && victoryFrames > 0){
     victoryScreen();
     victoryFrames--;
    }

    // Print "Paused" text when game is paused
    if(gameEngine->isPaused) {
      printText("PAUSED", 0.0, .60);
    }

    // Delete score character arrays
    delete p1ScoreStr;
    delete p2ScoreStr;

    // Calculate projection * view premultiplication
    glm::mat4 vp = projection * view;

    // Render shader objects
    for(int i = 0; i < NUM_OBJECTS; i++) {
      // Calculate mvp matrix
      mvp[i] = vp * gameEngine->models[i];

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
      int tex = 0;
      switch(i){
          // table
          case 0:
            if(gameEngine->pokemon) tex = 1;
            break;
          // paddle 1
          case 1:
            tex = 2;
            if(gameEngine->pokemon) tex = 4;
            break; 
          // paddle 2
          case 2:
            tex = 2;
            if(gameEngine->pokemon) tex = 3;
            break;
          // paddle 3
          case 3:
            tex = 2;
            if(gameEngine->pokemon) tex = 4;
            break;
          // paddle 4
          case 4:
            tex = 2;
            if(gameEngine->pokemon) tex = 3;
            break;
          // puck 1
          case 5:
            tex = 0;
            if(gameEngine->pokemon) tex = 1;
            break;
          // puck 2
          case 6:
            tex = 0;
            if(gameEngine->pokemon) tex = 1;
            break;
      }

      // Texture attribute
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, aTexture[tex]);
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

void Graphics::reshape(int sw, int sh){
    w = sw;
    h = sh;

    //Change the viewport to be correct
    glViewport( 0, 0, w, h);

    // Update the projection matrices for shader objects
    projection = glm::perspective( 45.0f, // FOV
                                   float(w)/float(h),
                                   0.01f, // Near plane
                                   1000.0f); // Far plane
}
      
bool Graphics::modelLoader(std::string fileName, int index){
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
   gameEngine->shape[index] = new btBvhTriangleMeshShape(objTriMesh, true);
   
   return true;
}
      
std::string Graphics::readShader(std::string arg){
   std::string shaderSrc;
   std::ifstream fin;
   std::string temp;

   // Open shader file
   fin.open(arg.c_str());
      
   // Read in shader
   while(!fin.eof()) {
      getline(fin,temp);
      shaderSrc.append(temp + '\n');                
   }

   fin.close();
   return shaderSrc;
}
      
void Graphics::printText( const char * string, float x, float y) {
    // No program needed to print text
    glUseProgram(0);

    char * str = new char[80];
    strcpy(str,string);
    char * home = str;

    // Display text
    glColor3f(0, 0, 0);
    glRasterPos2f(x,y);
    while(*str) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str++);
    }

    str = home;
    delete str;
}

