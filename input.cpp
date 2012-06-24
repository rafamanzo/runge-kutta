#include<cstdlib>
#include<cstdio>
#include<cstring>
#include "dataset.h"
#include "input.h"

using namespace runge_kutta;

Input::Input(char *file_name){
  fp = fopen(file_name, "r");
}

DataSet Input::parse(double *h, vector **v0, unsigned *v0_count){
  unsigned i, j, k;
  unsigned *n_x, *n_y, *n_z;
  vector_field *field;
  
  n_x = (unsigned *) malloc(sizeof(unsigned *));
  n_y = (unsigned *) malloc(sizeof(unsigned *));
  n_z = (unsigned *) malloc(sizeof(unsigned *));
  
  field = (vector_field *) malloc(sizeof(vector_field *));
  
  fscanf(fp, "%u", n_x);
  fscanf(fp, "%u", n_y);
  fscanf(fp, "%u", n_z);
  
  fscanf(fp, "%u", v0_count);
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
        fscanf(fp, "%lf", &(((*field)[DataSet::offset(*n_x, *n_y, i, j, k)]).x));
        fscanf(fp, "%lf", &(((*field)[DataSet::offset(*n_x, *n_y, i, j, k)]).y));
        fscanf(fp, "%lf", &(((*field)[DataSet::offset(*n_x, *n_y, i, j, k)]).z));
      }
    }
  }
  
  return DataSet(*n_x, *n_y, *n_z, *field);
}
