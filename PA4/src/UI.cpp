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
float check = 1.0;
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void menu(int);
void idle();

void initUI(){
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
    if(key == 'a'){ // Spin to the left
    check = 1.0;
    spin = -1.0;
    }
    if(key == 's'){ // Stop Rotation
    check = 0.0;
    }
    if(key == 'd'){ // Spin to the Right
    check = 1.0;
    spin = 1.0;
    }
    if(key == 'r'){ // Reverse Rotation
    check = 1.0;
    spin *= -1.0;
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
         break;
  
      case 3:   // Stop Rotation
         check = 0.0;
         break;

      case 4:   // Reverse Rotation
         check = 1.0;
         spin *= -1.0;
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
    }
    if(count == 3){
      glutPostRedisplay();
      count = 0;
    }
}
#endif
