#ifndef UI_CPP
#define UI_CPP
struct vec2{
  GLfloat x;
  GLfloat y;
};

typedef vec2 point2;

point2 points[3];
int count = 0;
int w = 640, h = 480;
float spin = 1;
static float rotAng = 0.0;
float check = 1.0;
string message = "Rotating Counter Clockwise";
string message1 = "Orbiting Counter Clockwise";
bool OrbitClock = true;
bool RotationClock = true;

void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void menu(int);
void special(int,int,int);
void screenText(float x, float y, char* text);
void idle();

void initUI(){
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse); 
    glutCreateMenu(menu);
    glutAddMenuEntry("quit",1);
    glutAddMenuEntry("start rotation",2);
    glutAddMenuEntry("stop rotation",3);
    glutAddMenuEntry("reverse rotation",4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void keyboard(unsigned char key, int x_pos, int y_pos)
{
    // Handle keyboard input
    if(key == 27){  // Exit Program
        exit(0);
    }

    if(key == 's'){ // Stop Rotation
    check = 0.0;
    message = "No Rotation";
    }

    if(key == 'r'){ // Reverse Rotation
    check = 1.0;
    RotationClock = !RotationClock;

      if(RotationClock)
        message = "Rotating Counter Clockwise";
      else
        message = "Rotating ClockWise";
    }
}

void menu(int id){
    switch(id){
      case 1:   // Exit Program
         exit(0);
         break;

      case 2:   // Start Rotation
         if(check == 0.0){spin = 1.0;}
         check = 1.0;
         message = "Rotating Counter Clockwise";
         break;
  
      case 3:   // Stop Rotation
         check = 0.0;
         message = "Rotating Clockwise";
         break;

      case 4:   // Reverse Rotation
         check = 1.0;
         RotationClock = !RotationClock;

         if(RotationClock)
           message = "Rotating Counter Clockwise";
         else
           message = "Rotating ClockWise";
    }
    glutPostRedisplay();
}

void idle(){
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){  // Reverse Rotation
      if(spin == 0.0){spin = 1.0;}
      else{spin *= -1.0;}
      count++;

      if(RotationClock)
        message = "Rotating Counter Clockwise";
      else
        message = "Rotating ClockWise";
    }
    if(count == 3){
      glutPostRedisplay();
      count = 0;
    }
}

void special(int key, int x, int y){

  switch(key){
     case GLUT_KEY_UP:
     OrbitClock = false;
     message1 = "Orbiting Clockwise";
     break;

     case GLUT_KEY_DOWN:
     OrbitClock = true;
     message1 = "Orbiting Counter Clockwise";
     break;

     case GLUT_KEY_LEFT:
     check = 1.0;
     RotationClock = false;
     message = "Rotating Clockwise";
     break;

     case GLUT_KEY_RIGHT:
     check = 1.0;
     RotationClock = true;
     message = "Rotating Counter Clockwise";
     break;
   }
}

void screenText(float x, float y, const char* text){

    glUseProgram(0);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(x,y);

    while(*text){
     glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *text);
     text++;
    }
}
#endif
