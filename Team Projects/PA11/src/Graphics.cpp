#include "Graphics.h"
#include "GameEngine.h"

// Constructor for graphics class
Graphics::Graphics(){
    w = 640; 
    h = 480; // Window size
    focusPoint = { 0.0, 0.0, 0.0 };
    tiltOffset = M_PI / 4;
}

// Destructor for graphics class
Graphics::~Graphics(){
}

// Initialize graphics class
bool Graphics::initialize(GameEngine* ptr){
    gameEngine = ptr; 
    cameraRadius = 20.0; // Distance from focus point (0,0,0)
    xAxisRadians = 0; // Latitude
    yAxisRadians = tiltOffset; // Longitude

    mvp = new glm::mat4[NUM_OBJECTS];
    gameEngine->models = new glm::mat4[NUM_OBJECTS];
    vbo_geometry = new GLuint[NUM_OBJECTS];
    aTexture = new GLuint[NUM_OBJECTS];
    geometryModels = new std::vector<Vertex>[NUM_OBJECTS];
    gameEngine->shape = new btCollisionShape*[NUM_OBJECTS];

    // Set object and texture names
    std::string objNames[NUM_OBJECTS] = {"table.obj", "sphere.obj"};
    std::string textNames[NUM_OBJECTS] = {"battle1.png","R2D2.jpg"};

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

    loc_norm = glGetAttribLocation(program, 
                    const_cast<const char*>("v_normal"));
    if(loc_norm == -1) {
        std::cerr << "[F] V_NORM NOT FOUND" << std::endl;
    }

    loc_lightPos = glGetUniformLocation(program, 
                    const_cast<const char*>("lightPosition"));
    if(loc_lightPos == -1) {
        std::cerr << "[F] LIGHTPOS NOT FOUND" << std::endl;
    }
    
    loc_spotDirection = glGetUniformLocation(program, 
                    const_cast<const char*>("spotlightDirection"));
    if(loc_lightPos == -1) {
        std::cerr << "[F] spotlightDirection NOT FOUND" << std::endl;
    }
    
    loc_spotCutoff = glGetUniformLocation(program, 
                    const_cast<const char*>("spotlightCutoff"));
    if(loc_lightPos == -1) {
        std::cerr << "[F] spotlightCutoff NOT FOUND" << std::endl;
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
    
    loc_aAttenParam = glGetUniformLocation(program, 
                    const_cast<const char*>("aAttenuationParam"));
    if(loc_shininess == -1) {
        std::cerr << "[F] aAttenuationParam NOT FOUND" << std::endl;
    }
    
    loc_bAttenParam = glGetUniformLocation(program, 
                    const_cast<const char*>("bAttenuationParam"));
    if(loc_shininess == -1) {
        std::cerr << "[F] bAttenuationParam NOT FOUND" << std::endl;
    }
    
    loc_minYLight = glGetUniformLocation(program, 
                    const_cast<const char*>("minYLight"));
    if(loc_shininess == -1) {
        std::cerr << "[F] minYLight NOT FOUND" << std::endl;
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
       // Size is number vertices * number positions, normals, colors * size of GLfloat
       glGenBuffers(1, &vbo_geometry[i]);
       glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry[i]);
       glBufferData(GL_ARRAY_BUFFER, geometryModels[i].size() * 8 * 4, &geometryModels[i][0], GL_STATIC_DRAW);//
    }
    for(int i = 0; i < NUM_OBJECTS; i++){
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
    eyePosition.x = focusPoint.x + cameraRadius * cos(yAxisRadians) * sin(xAxisRadians);
    eyePosition.y = focusPoint.y + cameraRadius * cos(yAxisRadians) * cos(xAxisRadians);
    eyePosition.z = focusPoint.z + cameraRadius * sin(yAxisRadians);

    // Initialize view matrix
    view = glm::lookAt( eyePosition, //Eye Position
                        focusPoint, //Focus point
                        glm::vec3(0.0, 0.0, -1.0)); //Positive Z is up

    // Initialize projection matrix
    projection = glm::perspective( 45.0f, // FOV
                                   float(w)/float(h),
                                   0.01f, // Near plane
                                   80.0f); // Far plane

   // Enable depth testing
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   return true;
}

// Callback function that updates camera view
void Graphics::update(){
    // Calculate eye position
    eyePosition.x = focusPoint.x + cameraRadius * cos(yAxisRadians) * sin(xAxisRadians);
    eyePosition.y = focusPoint.y + cameraRadius * cos(yAxisRadians) * cos(xAxisRadians);
    eyePosition.z = focusPoint.z + cameraRadius * sin(yAxisRadians);
    
    // Calculate focus point
    focusPoint = { gameEngine->models[1][3][0], 
                   gameEngine->models[1][3][1], 
                   gameEngine->models[1][3][2] };
    
    // Update camera view
    view = glm::lookAt(eyePosition, focusPoint, glm::vec3(0.0, 0.0, -1.0));
    
    // Call the display callback
    glutPostRedisplay();
}

void Graphics::updateLight() {
    // Ball light
    lightPosition[0] = glm::vec4(focusPoint.x, focusPoint.y + 5.0, focusPoint.z, 1.0);
    spotlightDirection[0] = glm::vec3(0.0, -1.0, 0.0);
    minYLight[0] = focusPoint.y - 5.0;
    
    if(gameEngine->ballLight) {
       spotlightCutoff[0] = 20.0;
    }
    else {
       spotlightCutoff[0] = 0.0;
    }

    // Start light
    lightPosition[1] = glm::vec4(0.0, 10.0, 0.0, 1.0);
    spotlightDirection[1] = glm::vec3(0.0, -1.0, 0.0);
    minYLight[1] = -5.0;
    spotlightCutoff[1] = 10.0;

    // Level 1 goal light
    lightPosition[2] = glm::vec4(-4.5, 5.0, 3.0, 1.0);
    spotlightDirection[2] = glm::vec3(0.0, -1.0, 0.0);
    minYLight[2] = -1.5;

    if(gameEngine->goalLight) {
       spotlightCutoff[2] = 20.0;
    }
    else {
       spotlightCutoff[2] = 0.0;
    }
    
    // Level 2 goal light
    lightPosition[3] = glm::vec4(-20.7, -77.5, 23.9, 1.0);
    spotlightDirection[3] = glm::vec3(0.0, -1.0, 0.0);
    minYLight[3] = -83.5;

    if(gameEngine->goalLight) {
       spotlightCutoff[3] = 20.0;
    }
    else {
       spotlightCutoff[3] = 0.0;
    }
    
    // Level 3 goal light
    lightPosition[4] = glm::vec4(51.0, -179.5, 96.0, 1.0);
    spotlightDirection[4] = glm::vec3(0.0, -1.0, 0.0);
    minYLight[4] = -185.5;

    if(gameEngine->goalLight) {
       spotlightCutoff[4] = 20.0;
    }
    else {
       spotlightCutoff[4] = 0.0;
    }
    
    // Ambient
    if(gameEngine->ambientLight) {
       ambientProduct = glm::vec4(0.1, 0.1, 0.1, 0.0); 
    }
    else {
       ambientProduct = glm::vec4(0.0, 0.0, 0.0, 0.0);
    }
    
    // Diffuse
    diffuseProduct = glm::vec4(0.5, 0.5, 0.5, 0.0);
    
    // Specular
    specularProduct = glm::vec4(0.1, 0.1, 0.1, 0.0);
    
    // Shininess
    shininess = 0.1;
    
    // Attenuation parameters
    aAttenuationParam = 0.0;
    bAttenuationParam = 0.0;
}

// Callback function that renders times and objects
void Graphics::render(){
    // Clear the screen
    glClearColor(.5, .5, .5, .5);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Print "Paused" text when game is paused
    if(gameEngine->isPaused) {
      printText("PAUSED", 0.0, 0.9);
    }

    if(gameEngine->victoryFrames > 0){
      printText("New High Score!!!!!", 0.0, 0.95);
      gameEngine->victoryFrames--;
    }
    
    // Print current time
    printText("Current Time:", -1.0, 0.95);
    std::stringstream temp_str;
    temp_str<<(gameEngine->getGameDuration());
    std::string timeStr = temp_str.str();
    printText(timeStr.c_str(), -0.85, 0.95);
    
    // Print best times
    if(gameEngine->displayTimes) {
       std::string rank[10] = {"1st","2nd","3rd","4th","5th","6th","7th","8th","9th","10th"};
       
       // Display text
       printText("Scoreboard", -0.95, 0.85);
       printText("___________________", -1.0, 0.825);
       
       // Display times
       for(int i=0; i<10; i++) {
          float yPos = 0.75 - float(i * 0.05);
          
          // Display rank
          printText(rank[i].c_str(), -1.0, yPos);
          printText("|", -.935, yPos);
          
          // Display time
          std::stringstream temp_str;
          temp_str<<(gameEngine->bestTimes[i]);
          std::string timeStr = temp_str.str();
          printText(timeStr.c_str(), -.9, yPos);
       }
    }
    
    // Light stuff
    updateLight();
    
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

      // Upload light data
      glUniform4fv(loc_lightPos, NUM_LIGHTS, glm::value_ptr(lightPosition[0]));
      glUniform3fv(loc_spotDirection, NUM_LIGHTS, glm::value_ptr(spotlightDirection[0]));
      glUniform1fv(loc_spotCutoff, NUM_LIGHTS, spotlightCutoff);
      glUniform4fv(loc_ambientProd, 1, glm::value_ptr(ambientProduct));
      glUniform4fv(loc_diffuseProd, 1, glm::value_ptr(diffuseProduct));
      glUniform4fv(loc_specularProd, 1, glm::value_ptr(specularProduct));
      glUniform1f(loc_shininess, shininess);
      glUniform1f(loc_aAttenParam, aAttenuationParam);
      glUniform1f(loc_bAttenParam, bAttenuationParam);
      glUniform1fv(loc_minYLight, NUM_LIGHTS, minYLight);
      
      // Set up the Vertex Buffer Object so it can be drawn
      glEnableVertexAttribArray(loc_position);
      glEnableVertexAttribArray(loc_texture);
      glEnableVertexAttribArray(loc_norm);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry[i]);

      // Set pointers into the vbo for each of the attributes

      // Position attribute
      glVertexAttribPointer( loc_position, // Location of attribute
                             3, // Number of elements
                             GL_FLOAT, // Type
                             GL_FALSE, // Normalized
                             sizeof(Vertex), // Stride
                             (void*)0); // Offset

      // Normal attribute
      glVertexAttribPointer( loc_norm,
                             3,
                             GL_FLOAT,
                             GL_FALSE,
                             sizeof(Vertex),
                             (void*)offsetof(Vertex,normal));
                             
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
      glDrawArrays(GL_TRIANGLES, 0, geometryModels[i].size()); // Mode, starting index, count

      // Clean up
      glDisableVertexAttribArray(loc_position);
      glDisableVertexAttribArray(loc_norm);
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
                                   80.0f); // Far plane
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

       // Check if normals exist
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

             triArray[j] = btVector3(position.x, position.y, position.z);
             
             geometryModels[index].push_back({ {position.x,
                                         position.y,
                                         position.z},
                                         {norm.x,
                                          norm.y,
                                          norm.z},
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
    glColor3f(1, 1, 1);
    glRasterPos2f(x,y);
    while(*str) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str++);
    }

    str = home;
    delete str;
}

