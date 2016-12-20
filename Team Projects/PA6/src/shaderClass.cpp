#include "shaderClass.h"

/**
Constructor for shader class.
*/
Shader::Shader() {
   // Create program
   program = glCreateProgram();
   
   // Set data members
   numVertices = 0;
   orbitAngle = 0.0;
   rotateAngle = 0.0;

   // Compute time member
   t1 = std::chrono::high_resolution_clock::now();
}

/**
Destructor for shader class.
*/
Shader::~Shader() {
   // Delete program
   glDeleteProgram(program);
   
   // Delete buffer
   glDeleteBuffers(1, &vbo_geometry);
}

/**
Render shader object. Called inside glutDisplayFunc callback function in main.cpp
*/
void Shader::render() {
    //--Render the scene

    //premultiply the matrix for this example
    mvp = projection * view * model;

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

///////////
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, aTexture); 
///////////

    glVertexAttribPointer( loc_texture,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,uv));



    glDrawArrays(GL_TRIANGLES, 0, numVertices);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_texture);    
}

/**
Perform translations on shader object. Called inside glutIdleFunc callback 
function in main.cpp

@param isMoon Boolean flag that is true when shader object is a moon
@param matrix model matrix used for translation
*/
void Shader::update() {
    //total time    
    float dt = getDT();// if you have anything moving, use dt.

    // Update rotatation and orbit
    orbitAngle += dt * M_PI/2;
    rotateAngle += dt * M_PI/2;

    // Translate model (orbit)
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0 * sin(orbitAngle), 0.0, 4.0 * cos(orbitAngle)));
    
    // Rotate model
    model = glm::rotate(model, rotateAngle, glm::vec3(0.0, 1.0, 0.0));    

    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}

/**
Reshape shader object based on window size. Called inside glutReshapeFunc 
callback function in main.cpp

@param width Width of the window
@param height Height of the window
*/
void Shader::reshape(int width, int height) {
    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(width)/float(height), 0.01f, 100.0f);
}

/**
Initialize shader object


@param width Width of the window
@param height Height of the window

@return Boolean flag is true when initialization is successful
*/
bool Shader::initialize(const char *path, const char* tpath, int width, int height) {
    // Load geometry
    Model geometry(path);
    numVertices = geometry.numVertices();

    // Check if .obj successfully loaded
    if(numVertices == 0) 
      return false;

    Magick::InitializeMagick(tpath);
    Magick::Image image;
    image.read(tpath);
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

    //--Geometry done
    
    // Create shader program
    program = buildProgram("shader.vs", "shader.fs");
    if(program == 0) {
      return false;
    }

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
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
    
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(width)/float(height), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane, 

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //and its done
    return true;
}

//returns the time delta
float Shader::getDT() {
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

/**
This function builds a GL program object and returns its reference. 

Vertex and fragment shader objects are created from source files defined by the
function's parameters. The GL objects are compiled then linked with the program
object. If an error occurs during file i/o, compilation, or linking, a value of
zero is returned.

@param vsFilename The filename for the vertex shader source code
@param fsFilename The filename for the fragment shader source code

@return GL program reference or 0 if error occured during program creation
*/
GLuint Shader::buildProgram(const char *vsFilename, const char *fsFilename) {
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
   if(!shader_status)
   {
      std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
      return 0;
   }
   
   return program;
}

