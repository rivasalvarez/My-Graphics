#include "CelestialBody.h"

// Constructor for CelestialBody class.
CelestialBody::CelestialBody() {
   // Create program
   program = glCreateProgram();

   // Set data members
   numVertices = 0;
   orbitAngle = 0.0;
   rotateAngle = 0.0;
   rotationSpeed = 2.0;
   orbitSpeed = 2.0;
   isPaused = false;
   isActualView = false;
}

// Constructor for CelestialBody class.
CelestialBody::CelestialBody(std::string name) {
   // Create program
   program = glCreateProgram();

   // Set data members
   objName = name;
   numVertices = 0;
   orbitAngle = 0.0;
   rotateAngle = 0.0;
   rotationSpeed = 2.0;
   orbitSpeed = 2.0;
   isPaused = false;
   isActualView = false;
}

// Destructor for CelestialBody class.
CelestialBody::~CelestialBody() {
   // Delete program
   glDeleteProgram(program);

   // Delete buffer
   glDeleteBuffers(1, &vbo_geometry);
   glDeleteBuffers(1, &vbo_orbit);
}

/**
Render CelestialBody object. Called inside glutDisplayFunc callback function in main.cpp

@param vp The premultiplied projection * view matrices
*/
void CelestialBody::render(const glm::mat4 vp) {
    // Premultiply the matrix for this example
    mvp = vp * model;

    // Enable the CelestialBody program
    glUseProgram(program);

    // Upload the matrix to the CelestialBody
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    // Set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    
    // Set pointers into the vbo for each of the attributes
    
    // Position attribute
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    // Texture attribute
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, aTexture);
    glVertexAttribPointer( loc_texture,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,uv));

    // Draw object
    glDrawArrays(GL_TRIANGLES, 0, numVertices);//mode, starting index, count

    // Clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_texture);
}

/**
Perform translations on CelestialBody object. Called inside glutIdleFunc callback
function in main.cpp

@param dt Time used to calculate rotation angle and orbit angle
@param matrix Model matrix of the celestial body's parent object
@param bodyOfRotation Name of the parent celestial body object
*/
void CelestialBody::update(float dt, glm::mat4 matrix, std::string bodyOfRotation) {
    // Determine view
    float d, s;

    // If current view mode uses actual scale and distance
    if(isActualView) {
        d = actualDistance;
        s = actualScale;
    }
   
    // Otherwise, use scaled distance and scale
    else {
        d = distance;
        s = scale;
    }

    // If model is not paused
    if(!isPaused) {
       // Rotate model
       rotateAngle += dt * M_PI * (rotationSpeed/10);

       // Orbit model counter-clockwise 90 degrees a second
       orbitAngle += dt * M_PI * (orbitSpeed/30);   //orbit speed divided by 10 slow down planets
    }

    // Begin translating and rotating bodies

    // If celestial body is a moon of uranus, it should translate along the y axis.
    if(bodyOfRotation == "Uranus") {
       translateModel = glm::translate(matrix, glm::vec3(d * sin(orbitAngle), d * cos(orbitAngle), 0.0));  
    }
    
    // If celestial body is pluto, it should translate  along the x y z axis.
    else if(objName == "Pluto") {
       translateModel = glm::translate(matrix, glm::vec3(d * sin(orbitAngle), d * sin(orbitAngle) / 4.0, d * cos(orbitAngle)));
    }
    
    // Else, Translate celestial body along the x-z plane
    else {
       translateModel = glm::translate(matrix, glm::vec3(d * sin(orbitAngle), 0.0, d * cos(orbitAngle)));
    }
    
    // If uranus or a moon of uranus, it rotates on its z axis.
    if(objName == "Uranus" || bodyOfRotation == "Uranus") {
       model = glm::rotate(translateModel, rotateAngle, glm::vec3(0.0, 0.0, 1.0));
    }
    
    // Else Rotate model
    else {
       model = glm::rotate(translateModel, rotateAngle, glm::vec3(0.0, 1.0, 0.0));
    }
    
    // Finally, scale the model appropriately.
    model = glm::scale(model, glm::vec3(s));

    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}

/**
Initialize CelestialBody object

@param width Width of the window
@param height Height of the window

@return Boolean flag is true when initialization is successful
*/
bool CelestialBody::initialize(int width, int height) {
    // Load specifications for celestial body
    if(!setSpecifications()) {
       std::cerr << "Celestial body tag '" << objName << "' not found in specifications file" << std::endl;
       return false;
    }

    // Load geometry
    Model geometry(objPath);
    numVertices = geometry.numVertices();

    // Check if .obj successfully loaded
    if(numVertices == 0)
      return false;

    // Load texture for object
    Magick::InitializeMagick(textName.c_str());
    Magick::Image image;
    image.read(textName.c_str());
    Magick::Blob blob;
    image.write(&blob,"RGBA");

    glGenTextures(1, &aTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, aTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.columns(), image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, geometry.size(), &(geometry.getModel())[0], GL_STATIC_DRAW);

    // Create CelestialBody program
    program = buildProgram("shader.vs", "shader.fs");
    if(program == 0) {
      return false;
    }

    // Set the locations of the attributes and uniforms
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1) {
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
    if(loc_mvpmat == -1) {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }

    // Create VBO for orbit path
    
    // Get vertices of orbit path
    std::vector<glm::vec3> vertices;

    // If celestial body not Pluto, create orbit path for celestial body
    if(objName != "Pluto") {
       for(int i=0; i<360; i++) {
           float radians = i * (M_PI / 180);
           glm::vec3 tempVertex = { cos(radians), 0.0, sin(radians) };
           vertices.push_back(tempVertex);
       }
    }
    
    // Otherwise, create unique orbit path for Pluto
    else {
       for(int i=0; i<360; i++) {
           float radians = i * (M_PI / 180);
           glm::vec3 tempVertex = { cos(radians), cos(radians) / 4.0, sin(radians) };
           vertices.push_back(tempVertex);
       }
    }
    
    // Create a Vertex Buffer object to store this orbit vertex info on the GPU
    glGenBuffers(1, &vbo_orbit);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_orbit);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW); 

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return true;
}

// Return CelestialBody object model after translation
glm::mat4 CelestialBody::getTranslateModel() {
   return translateModel;
}

// Return CelestialBody object model
glm::mat4 CelestialBody::getModel() {
   return model;
}

// Return CelestialBody object index
int CelestialBody::getObjIndex(){
   return objIndex;
}

// Pauses/resumes the solar system simulation
void CelestialBody::pause() {
   isPaused = !isPaused;
}

// Changes the solar system view between scaled and actual
void CelestialBody::toggleView() {
   isActualView = !isActualView;
}

// Reads in the celestial body specifications from an input file
bool CelestialBody::setSpecifications() {
   bool isFound = false;

   // Open scale specifications file
   std::ifstream file("CelestialBodySpecifications");
   if(!file.is_open()) {
      std::cerr << "[F] FAILED TO OPEN THE SPECIFICATIONS FILE FOR " << objName << std::endl;
      return false;
   }

   // Read in initial comment line/prime loop
   std::string temp;
   getline(file, temp);

   // Set specifications for celestial body
   while(!file.eof()) {
      // Check if tag found
      if(temp == objName) {
         // Set specifications
         file >> objIndex >> rotationSpeed >> orbitSpeed >> scale >> distance >> actualScale >> actualDistance;
         //multiplied by 2 for accuracy
         actualDistance = actualDistance * 2.0;
         std::string name;
         file >> name;
         textName = "Textures/" + name;
         file >> name;
         objPath = "Models/" + name;
         isFound = true;
         break;
      }

      // Otherwise, continue searching
      else
         file >> temp;
   }

   // Close file
   file.close();

   // Return flag
   return isFound;
}

/**
This function builds a GL program object and returns its reference.

Vertex and fragment CelestialBody objects are created from source files defined by the
function's parameters. The GL objects are compiled then linked with the program
object. If an error occurs during file i/o, compilation, or linking, a value of
zero is returned.

@param vsFilename The filename for the vertex CelestialBody source code
@param fsFilename The filename for the fragment CelestialBody source code

@return GL program reference or 0 if error occured during program creation
*/
GLuint CelestialBody::buildProgram(const char *vsFilename, const char *fsFilename) {
   GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
   GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
   GLint shader_status;

   // Open source files
   std::ifstream vsFile(vsFilename);
   std::ifstream fsFile(fsFilename);

   // If vertex shader source file correctly open
   if(vsFile.is_open()) {
      // Read file lines
      std::string vsString, fileLine;
      while(getline(vsFile, fileLine)) {
         // Append file line to file string
         vsString.append(fileLine);
         vsString.append("\n");
      }

      // Close file
      vsFile.close();

      // Compile vertex shader
      const char *vs = vsString.c_str();
      glShaderSource(vertex_shader, 1, &vs, NULL);
      glCompileShader(vertex_shader);

      // Check the compile status
      glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
      if(!shader_status) {
         std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
         return 0;
      }
   }

   // Otherwise, return error
   else {
          std::cerr << "[F] FAILED TO OPEN VERTEX SHADER FILE!" << std::endl;
          return 0;
   }

   // If fragment shader source file correctly open
   if(fsFile.is_open()) {
      // Read file lines
      std::string fsString, fileLine;
      while(getline(fsFile, fileLine)) {
         // Append file line to file string
         fsString.append(fileLine);
         fsString.append("\n");
      }

      // Close file
      fsFile.close();

      // Compile fragment shader
      const char *fs = fsString.c_str();
      glShaderSource(fragment_shader, 1, &fs, NULL);
      glCompileShader(fragment_shader);

      // Check the compile status
      glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
      if(!shader_status)
      {
         std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
         return 0;
      }
   }

   // Otherwise, return error
   else {
      std::cerr << "[F] FAILED TO OPEN FRAGMENT SHADER FILE!" << std::endl;
      return 0;
   }

   // Link shader objects with program
   glAttachShader(program, vertex_shader);
   glAttachShader(program, fragment_shader);
   glLinkProgram(program);

   // Check the link status
   glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
   if(!shader_status) {
      std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
      return 0;
   }

   return program;
}

/**
Helper function that prints text to the screen.

@param text The text to display on the screen
@param x The x-coordinate of the text
@param y The y-corrdinate of the text
*/
void CelestialBody::printText(const char *text, float x, float y) {
    // No program needed to print text
    glUseProgram(0);

    // Display text
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(x,y);
    while(*text) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *text++);
    }
}

// Return the celestial body object name
std::string CelestialBody::getObjName() {
	return objName;
}

/**
Initialize celestial body information

@param numS The number of strings to store for the celestial body
@param tempS The array of strings to store for the celestial body
*/
void CelestialBody::initInfo(int numS, std::string tempS[]) {
    // Set and Allocate variables
    numInfoStrings = numS;
    infoStrings = new std::string[numS];

    // Set string information
    for(int i = 0; i < numS; i++)
      infoStrings[i] = tempS[i];
}

// Prints celestial body information to screen
void CelestialBody::printInfo(){
   // Set x and y locations
   float x = -.99;
   float y =  .95;

   // Print information
   for(int i = 0; i < numInfoStrings; i++) {
       printText(infoStrings[i].c_str(),x,y);
       y -= .05;
   }
}

/**
Draws orbit path of the celestial body.

@param vp The premultiplied projection * view matrices
*/
void CelestialBody::drawOrbit(const glm::mat4 vp) {   
    float d;
    
    // If current view uses actual distance
    if(isActualView) {
        d = actualDistance;
    }
    
    // Otherwise, use scaled distance
    else {
        d = distance;
    }

    // Calculate MVP for orbit    
    glm::mat4 orbitMVP = vp * glm::scale(glm::mat4(1.0f), glm::vec3(d));
    
    // Use program
    glUseProgram(program);
        
    // Upload the matrix to the CelestialBody
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(orbitMVP));

    // Set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_orbit);
    
    // Set pointers into the vbo for each of the attributes
    
    // Position attribute
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
 
    // Texture attribute
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, aTexture);
    glVertexAttribPointer(loc_texture, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,uv));
    
    // Draw circle
    glDrawArrays(GL_LINE_LOOP, 0, 360);

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_texture);
}
