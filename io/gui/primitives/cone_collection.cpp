#include<cstdlib>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include<dataset.h>
#include<cone.h>
#include<cone_collection.h>

using namespace runge_kutta;

ConeCollection::ConeCollection(){
  _cone_count = 0;
  _cones = NULL;
}

void ConeCollection::addCone(vector p1, vector p2){
  _cones = (Cone *) realloc(_cones, (_cone_count + 1)*sizeof(Cone));
  _cones[_cone_count] = Cone(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
  _cone_count++;
}

void ConeCollection::render(GLfloat red, GLfloat green, GLfloat blue, GLdouble angle_x, GLdouble angle_y){
  unsigned cone;
  
  for(cone = 0; cone < _cone_count; cone++)
    _cones[cone].render(red, green, blue, angle_x, angle_y);
}
