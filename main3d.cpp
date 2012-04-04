#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "vector_field.h"
#include "rk3d.h"
#include "input3d.h"
#include "output.h"

int main(int argc, char *argv[]){
  char input[50];
  vector_field field;
  int n_x, n_y, n_z, v0_count, i;
  vector *v0;
  double h;
  vector **points_rk4, **points_rk2;
  int *n_points_rk4, *n_points_rk2;
  
  strcpy(input, argv[1]);
  parseFile(input, &n_x, &n_y, &n_z, &h, &v0, &v0_count, &field);

  /*printf("\nv0_count: %d", v0_count);
  for(i = 0; i < v0_count; i++)
    printf("\n(%f, %f, %f)", v0[i].x, v0[i].y, v0[i].z);
  printf("\nn_x: %d | n_y: %d | n_z: %d | h: %f", n_x, n_y, n_z, h);
  for(k = 0; k < n_z; k++){
    printf("\n**********");
    for(i = 0; i < n_x; i++){
      printf("\n");
      for(j = 0; j < n_y; j++){
        printf("\n(%f, %f, %f)", field[offset(n_x, n_y, i, j, k)].x, field[offset(n_x, n_y, i, j, k)].y, field[offset(n_x, n_y, i, j, k)].z);
      }
    }
  }*/

  points_rk2 = (vector **) malloc(v0_count*sizeof(vector *));
  n_points_rk2 = (int *) malloc(v0_count*sizeof(int));
  points_rk4 = (vector **) malloc(v0_count*sizeof(vector *));
  n_points_rk4 = (int *) malloc(v0_count*sizeof(int));
  
  rk2(v0, v0_count, h, n_x, n_y, n_z, field, &points_rk2, &n_points_rk2);
  /*rk4(v0, v0_count, h, n_x, n_y, n_z, field, &points_rk4, &n_points_rk4);
  
  for(i = 0; i < v0_count; i++)
    generate_gnuplot_input(n_x, n_y, n_z, n_points_rk2[i], points_rk2[i], n_points_rk4[i], points_rk4[i]);*/
  
  return 0;
}
