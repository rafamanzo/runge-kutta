#include<cstdlib>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include<dataset.h>
#include<cylinder.h>
#include<cylinder_collection.h>

using namespace runge_kutta;

CylinderCollection::CylinderCollection(){
  _cylinder_count = 0;
  _cylinders = NULL;
}

void CylinderCollection::addCylinder(vector p1, vector p2){
  _cylinders = (Cylinder *) realloc(_cylinders, (_cylinder_count + 1)*sizeof(Cylinder));
  _cylinders[_cylinder_count] = Cylinder(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
  _cylinder_count++;
}

void CylinderCollection::render(GLfloat red, GLfloat green, GLfloat blue, GLdouble angle_x, GLdouble angle_y){
  unsigned cylinder;
  
  for(cylinder = 0; cylinder < _cylinder_count; cylinder++)
    _cylinders[cylinder].render(red, green, blue, angle_x, angle_y);
}
