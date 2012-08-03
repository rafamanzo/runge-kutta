#include<GL/glut.h>
#include<dataset.h>
#include<fiber.h>
#include<cylinder.h>
#include<cylinder_collection.h>
#include<cone.h>
#include<cone_collection.h>
#include<scene.h>
#include<window_manager.h>

using namespace runge_kutta;

Scene _scene;

WindowManager::WindowManager(Scene scene){
  int argc = 0;
  char *argv[1];
  
  _scene = scene;

  glutInit(&argc, argv);
  glutInitWindowSize(800,600);
  glutInitWindowPosition(0,0);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Runge-Kutta");
  glViewport(0, 0, 800, 600);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(key_pressed);
  glutSpecialFunc(key_pressed_special);
}

void WindowManager::loop(){
  glutMainLoop();
}

void WindowManager::display(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  
  _scene.render();
   
  glutSwapBuffers();
}

void WindowManager::reshape(int width, int height){
  glViewport (0, 0, (GLsizei) width, (GLsizei) height); 
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0,((float) width)/((float) height),0.01,5000);
  glMatrixMode (GL_MODELVIEW);
}

void WindowManager::idle(){
  glutPostRedisplay(); 
}

void WindowManager::key_pressed (unsigned char key, int x, int y) {  
  if(key == '+')
    _scene.decreaseScale();
  else if(key == '-')
    _scene.increaseScale();
  else if(key == 's')
    _scene.increaseY();
  else if(key == 'w')
    _scene.decreaseY();
  else if(key == 'a')
    _scene.decreaseX();
  else if(key == 'd')
    _scene.increaseX();
  else if(key == '2')
    _scene.toogleRK2();
  else if(key == '4')
    _scene.toogleRK4();
  else if(key == 'f')
    _scene.toogleDS();
}

void WindowManager::key_pressed_special (int key, int x, int y) {  
  if(key == GLUT_KEY_LEFT)
    _scene.decreaseYAngle();
  else if(key == GLUT_KEY_RIGHT)
    _scene.increaseYAngle();
  else if(key == GLUT_KEY_UP)
    _scene.decreaseXAngle();
  else if(key == GLUT_KEY_DOWN)
    _scene.increaseXAngle();
}
