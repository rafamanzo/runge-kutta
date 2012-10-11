#include<cstdlib>
#include<cmath>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include<dataset.h>
#include<fiber.h>
#include<cylinder.h>
#include<cylinder_collection.h>
#include<cone.h>
#include<cone_collection.h>
#include<scene.h>

#define MIN_FIELD_VEC_MAG 0.000001

using namespace runge_kutta;

Scene::Scene(){
  _x_angle = 0.0;
  _y_angle = 0.0;
  _translation_x = 0.0;
  _translation_y = 0.0;
  _translation_z = 0.0;
  _display_rk2 = true;
  _display_rk4 = true;
  _display_vf = false;
}

Scene::Scene(DataSet data_set, unsigned fibers_count, Fiber *rk2_fibers, Fiber *rk4_fibers){
  unsigned fiber_index, point_index, i, j, k;
  Fiber fiber;
  vector initial_point, final_point;
  
  _x_angle = 0.0;
  _y_angle = 0.0;
  _translation_x = 0.0;
  _translation_y = 0.0;
  _translation_z = 0.0;
  _display_rk2 = true;
  _display_rk4 = true;
  _display_vf = false;
    
  //Fibers abstraction
  for(fiber_index = 0; fiber_index < fibers_count; fiber_index++){
    //RK2
    fiber = rk2_fibers[fiber_index];
    for(point_index = 1; point_index < fiber.pointsCount(); point_index++){
      initial_point = fiber.getPoint(point_index - 1);
      final_point = fiber.getPoint(point_index);
      _rk2_cylinders.addCylinder(initial_point, final_point);
    }
    
    //RK4
    fiber = rk4_fibers[fiber_index];
    for(point_index = 1; point_index < fiber.pointsCount(); point_index++){
      initial_point = fiber.getPoint(point_index - 1);
      final_point = fiber.getPoint(point_index);
      _rk4_cylinders.addCylinder(initial_point, final_point);
    }
  }
  
  //Dataset abstraction
  for(i = 0; i < data_set.n_x(); i++)
    for(j = 0; j < data_set.n_y(); j++)
      for(k = 0; k < data_set.n_z(); k++){
        initial_point.x = (double) i;
        initial_point.y = (double) j;
        initial_point.z = (double) k;
        
        final_point.x = (double) (i + data_set.field(i, j, k).x);
        final_point.y = (double) (j + data_set.field(i, j, k).y);
        final_point.z = (double) (k + data_set.field(i, j, k).z);
        
        if(sqrt(data_set.field(i, j, k).x*data_set.field(i, j, k).x +
                data_set.field(i, j, k).y*data_set.field(i, j, k).y +
                data_set.field(i, j, k).z*data_set.field(i, j, k).z
               ) > MIN_FIELD_VEC_MAG)
          _vector_field.addCone(initial_point, final_point);
      }
}

void Scene::render(){
  glPushMatrix();
    glLoadIdentity();
    
    gluLookAt(0, 0,-150, 0,0,0, 0,1,0);
    //TODO: ilumination();    
    
    glTranslated(_translation_x, _translation_y, _translation_z);
    
    renderAxis();
    renderVectorField();
    renderCylinders();
  glPopMatrix();
}

void Scene::renderVectorField(){
  if(_display_vf) _vector_field.render(0.0,0.0,1.0, _x_angle, _y_angle);
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
  _translation_z += 1; 
}

void Scene::decreaseScale(){
  _translation_z -= 1;
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

void Scene::toogleDS(){
  _display_vf = !_display_vf;
}
