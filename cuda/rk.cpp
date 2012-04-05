#include<stdlib.h>
#include "../vector_field.h"
#include "../rk.h"
#include "rk_kernel.h"

void rk2(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  rk2_caller(v0, count_v0, h, n_x, n_y, n_z, field, points, n_points);
}

void rk4(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  rk4_caller(v0, count_v0, h, n_x, n_y, n_z, field, points, n_points);
}
