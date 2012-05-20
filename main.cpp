#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "vector_field.h"
#include "rk.h"
#include "input.h"
#include "output.h"
#include "opengl/plot.h"

int main(int argc, char *argv[]){
  char input[50];
  char c;
  vector_field field;
  int n_x, n_y, n_z, v0_count, i;
  vector *v0;
  double h;
  vector **points_rk4, **points_rk2;
  int *n_points_rk4, *n_points_rk2;
  
  strcpy(input, argv[1]);
  parseFile(input, &n_x, &n_y, &n_z, &h, &v0, &v0_count, &field);
  
  rk4(v0, v0_count, h, n_x, n_y, n_z, field, &points_rk4, &n_points_rk4);
  rk2(v0, v0_count, h, n_x, n_y, n_z, field, &points_rk2, &n_points_rk2);


  printf("Press \"1\" to generate gnuplot input;\nPress \"2\" to plot the solution;\nPress \"Any other caracter\" to exit\n");
  c = getchar();
  if( c == '1' ){
    generate_gnuplot_input(n_x, n_y, n_z, v0_count, n_points_rk2, points_rk2, n_points_rk4, points_rk4);
    printf("Gnuplot input generated with success.\n"); 
  }
  else if( c== '2'){
    //plot_main(argc,argv,n_x,n_y,n_z,v0_count,n_points_rk2, points_rk2);
    plot_main(argc,argv,n_x,n_y,n_z,v0_count,n_points_rk4, points_rk4);
  }

  free(v0);
  free(n_points_rk4);
  free(n_points_rk2);
  for(i = 0; i < v0_count; i++){
    free(points_rk4[i]);
    free(points_rk2[i]);
  }

  return 0;
}
