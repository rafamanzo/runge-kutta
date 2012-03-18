#include<stdlib.h>
#include<math.h>
#include "vector_field.h"

int offset(int n_x, int n_y, int x, int y, int z){
  return x + n_x*y + n_y*n_x*z;
}

vector sum(vector v1, vector v2){
  vector sum;
  
  sum.x = v1.x + v2.x;
  sum.y = v1.y + v2.y;
  sum.z = v1.z + v2.z;
  
  return sum;
}

vector mult_scalar(vector v, double scalar){
  vector mult;
  
  mult.x = v.x*scalar;
  mult.y = v.y*scalar;
  mult.z = v.z*scalar;
  
  return mult;
}



double module(vector v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}
