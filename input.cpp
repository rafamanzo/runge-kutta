#include<cstdlib>
#include<cstdio>
#include<cstring>
#include "vector_field.h"
#include "input.h"

using namespace runge_kutta;

Input::Input(char *file_name){
  fp = fopen(file_name, "r");
}

void Input::parse(int *n_x, int *n_y, int *n_z, double *h, vector **v0, int *v0_count, vector_field *field){
  int i, j, k;
  
  fscanf(fp, "%d", n_x);
  fscanf(fp, "%d", n_y);
  fscanf(fp, "%d", n_z);
  
  fscanf(fp, "%d", v0_count);
  *v0 = (vector *) malloc( (*v0_count)*sizeof(vector) );
  for(i = 0; i < *v0_count; i++){
    fscanf(fp, "%lf", &(((*v0)[i]).x));
    fscanf(fp, "%lf", &(((*v0)[i]).y));
    fscanf(fp, "%lf", &(((*v0)[i]).z));
  }
  fscanf(fp, "%lf", h);
  
  *field = (vector_field) malloc( (*n_x)*(*n_y)*(*n_z)*sizeof(vector) );
  
  for(k = 0; k < *n_z; k++){
    for(i = 0; i < *n_x; i++){
      for(j = 0; j < *n_y; j++){
        fscanf(fp, "%lf", &(((*field)[offset(*n_x, *n_y, i, j, k)]).x));
        fscanf(fp, "%lf", &(((*field)[offset(*n_x, *n_y, i, j, k)]).y));
        fscanf(fp, "%lf", &(((*field)[offset(*n_x, *n_y, i, j, k)]).z));
      }
    }
  }
}
