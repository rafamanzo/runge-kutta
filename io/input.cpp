#include<cstdlib>
#include<cstdio>
#include<cstring>
#include<CImg.h>
#include<dataset.h>
#include<input.h>

using namespace runge_kutta;
using namespace cimg_library;

Input::Input(char *file_name, unsigned file_type){
  _file_name = (char *) malloc(strlen(file_name)*sizeof(char));
  strcpy(_file_name, file_name);
  _file_type = file_type;
}

Input::Input(){
  _file_name = NULL;
  _file_type = 2;
}

DataSet Input::parse(double *h, vector **v0, unsigned *v0_count){
  if(_file_type == NATIVE_TYPE)
    return parseNative(h,v0,v0_count);
  else if(_file_type == ANALYZE_TYPE)
    return parseAnalyze(h,v0,v0_count);
    
  return DataSet();
}

DataSet Input::parseNative(double *h, vector **v0, unsigned *v0_count){
  unsigned i, j, k;
  unsigned *n_x, *n_y, *n_z;
  vector_field *field;
  FILE *fp;
  
  fp = fopen(_file_name, "r");
  
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

DataSet Input::parseAnalyze(double *h, vector **v0, unsigned *v0_count){
  unsigned i, j, k;
  unsigned *n_x, *n_y, *n_z;
  double multiplier;
  vector_field *field;
  
  CImg<float> img(_file_name);
  
  n_x = (unsigned *) malloc(sizeof(unsigned *));
  n_y = (unsigned *) malloc(sizeof(unsigned *));
  n_z = (unsigned *) malloc(sizeof(unsigned *));
  
  field = (vector_field *) malloc(sizeof(vector_field *));
  
  *n_x = img.width();
  *n_y = img.height();
  *n_z = img.depth();
  
  printf("\nPlease enter the initial points count:\n");
  scanf("%u", v0_count);
  *v0 = (vector *) malloc( (*v0_count)*sizeof(vector) );
  for(i = 0; i < *v0_count; i++){
    printf("\nPlease enter the %dth initial points coordinates:\n", i);
    scanf("%lf", &(((*v0)[i]).x));
    scanf("%lf", &(((*v0)[i]).y));
    scanf("%lf", &(((*v0)[i]).z));
  }
  printf("\nPlease enter the step size:\n");
  scanf("%lf", h);
  
  printf("\nPlease enter and multiplier for the vector magnitude:\n");
  scanf("%lf", &multiplier);
  
  *field = (vector_field) malloc( (*n_x)*(*n_y)*(*n_z)*sizeof(vector) );
  
  for(k = 0; k < *n_z; k++){
    for(i = 0; i < *n_x; i++){
      for(j = 0; j < *n_y; j++){
        ((*field)[DataSet::offset(*n_x, *n_y, i, j, k)]).x = img(i,j,k,0)*multiplier;
        ((*field)[DataSet::offset(*n_x, *n_y, i, j, k)]).y = img(i,j,k,1)*multiplier;
        ((*field)[DataSet::offset(*n_x, *n_y, i, j, k)]).z = img(i,j,k,2)*multiplier;
      }
    }
  }
  
  return DataSet(*n_x, *n_y, *n_z, *field);
}
