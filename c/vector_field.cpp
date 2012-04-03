#include<stdlib.h>
#include<math.h>
#include "../vector_field.h"

int offset(int n_x, int n_y, int x, int y, int z){
  return x + n_x*y + n_y*n_x*z;
}
