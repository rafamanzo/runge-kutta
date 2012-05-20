#include <math.h>
#include "../vector_field.h"
#include "math_operations.h"

#ifndef PI
#define PI 3.141592653589793
#endif

double angle_x(vector v){
  return atan2(v.y,v.z)*180/PI;
}

double angle_y(vector v){
  return atan2(v.x,v.z)*180/PI;
}

double angle_z(vector v){
  return atan2(v.x,v.y)*180/PI;
}

double module2(vector v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

