#include <cstdlib>
#include <GL/glut.h>
#include "../../core/dataset.h"
#include "../../core/fiber.h"
#include "primitives/cylinder.h"
#include "primitives/cylinder_collection.h"
#include "scene.h"

using namespace runge_kutta;

Scene::Scene(){
  _scale = 1.0;
  _x_angle = 0.0;
  _y_angle = 0.0;
  _translation_x = 0.0;
  _translation_y = 0.0;
  _display_rk2 = true;
  _display_rk4 = true;
}

Scene::Scene(unsigned fibers_count, Fiber *rk2_fibers, Fiber *rk4_fibers){
  unsigned fiber_index, point_index;
  Fiber fiber;
  vector initial_point, final_point;
  
  _scale = 1.0;
  _x_angle = 0.0;
  _y_angle = 0.0;
  _translation_x = 0.0;
  _translation_y = 0.0;
  _display_rk2 = true;
  _display_rk4 = true;
  
  for(fiber_index = 0; fiber_index < fibers_count; fiber_index++){
    fiber = rk2_fibers[fiber_index];
    for(point_index = 1; point_index < fiber.pointsCount(); point_index++){
      initial_point = fiber.getPoint(point_index - 1);
      final_point = fiber.getPoint(point_index);
      _rk2_cylinders.addCylinder(initial_point, final_point);
    }
    
    fiber = rk4_fibers[fiber_index];
    for(point_index = 1; point_index < fiber.pointsCount(); point_index++){
      initial_point = fiber.getPoint(point_index - 1);
      final_point = fiber.getPoint(point_index);
      _rk4_cylinders.addCylinder(initial_point, final_point);
    }
  }
}

void Scene::render(){
  glPushMatrix();
    glLoadIdentity();
    
    gluLookAt(0, 0,-150, 0,0,0, 0,1,0);
    //TODO: ilumination();    
    
    glScaled(_scale, _scale, _scale);
    glTranslated(_translation_x, _translation_y, 0.0);
    
    renderAxis();
    renderCylinders();
  glPopMatrix();
}

void Scene::renderAxis(){
  glPushMatrix();
    glColor3f(0.0,0.0,0.0);

    glRotated(_x_angle, 1.0, 0.0, 0.0);
    glRotated(_y_angle, 0.0, 1.0, 0.0);
    
    glBegin(GL_LINES);
      glVertex3f(0.0,0.0,0.0);
      glVertex3f(300.0,0.0,0.0);
    glEnd();
    
    glBegin(GL_LINES);
      glVertex3f(0.0,0.0,0.0);
      glVertex3f(0.0,300.0,0.0);
    glEnd();
    
    glBegin(GL_LINES);
      glVertex3f(0.0,0.0,0.0);
      glVertex3f(0.0,0.0,300.0);
    glEnd();
  glPopMatrix();
}

void Scene::renderCylinders(){
  glPushMatrix();
    if(_display_rk2) _rk2_cylinders.render(0.0, 1.0, 0.0, _x_angle, _y_angle);
    if(_display_rk4) _rk4_cylinders.render(1.0, 0.0, 0.0, _x_angle, _y_angle);
  glPopMatrix();
}

void Scene::increaseXAngle(){
  _x_angle += 1; 
}

void Scene::decreaseXAngle(){
  _x_angle -= 1; 
}

void Scene::increaseYAngle(){
  _y_angle += 1; 
}

void Scene::decreaseYAngle(){
  _y_angle -= 1; 
}

void Scene::increaseX(){
  _translation_x += 1; 
}

void Scene::decreaseX(){
  _translation_x -= 1; 
}

void Scene::increaseY(){
  _translation_y += 1; 
}

void Scene::decreaseY(){
  _translation_y -= 1; 
}

void Scene::increaseScale(){
  _scale += 0.05; 
}

void Scene::decreaseScale(){
  _scale -= 0.05; 
}

void Scene::ilumination(){
  const GLfloat light_ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat light_position[] = { 0.0f, 0.0f, -1250.0f, 0.0f };
  const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
  const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
  const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat high_shininess[] = { 100.0f };
  
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
}

void Scene::toogleRK2(){
  _display_rk2 = !_display_rk2;
}

void Scene::toogleRK4(){
  _display_rk4 = !_display_rk4;
}
