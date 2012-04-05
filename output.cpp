#include<stdlib.h>
#include<stdio.h>
#include "vector_field.h"
#include "output.h"

void generate_gnuplot_input(int n_x, int n_y, int n_z, int v0_count, int *n_points_rk2, vector **points_rk2, int *n_points_rk4, vector **points_rk4){
  FILE *rk2_dat, *rk4_dat, *rk2_vs_rk4;
  int i,k;
  
  rk2_dat = fopen("rk2.dat", "w");
  rk4_dat = fopen("rk4.dat", "w");
  rk2_vs_rk4 = fopen("rk2-vs-rk4.p", "w");
  
  for(k = 0; k < v0_count; k++){
    for(i = 0; i < n_points_rk2[k]; i++)
      fprintf(rk2_dat, "%f %f %f\n", points_rk2[k][i].x, points_rk2[k][i].y, points_rk2[k][i].z);
    fprintf(rk2_dat, "\n");
    
    for(i = 0; i < n_points_rk4[k]; i++)
      fprintf(rk4_dat, "%f %f %f\n", points_rk4[k][i].x, points_rk4[k][i].y, points_rk4[k][i].z);
    fprintf(rk4_dat, "\n");
  }
  
  fclose(rk2_dat);
  fclose(rk4_dat);
  
  fprintf(rk2_vs_rk4, "set xrange[0:%d]\n", n_x - 1);
  fprintf(rk2_vs_rk4, "set yrange[0:%d]\n", n_y - 1);
  fprintf(rk2_vs_rk4, "set zrange[0:%d]\n", n_z - 1);
  fprintf(rk2_vs_rk4, "splot \"rk2.dat\" w lines lt 1, \"rk4.dat\" w lines lt 2\n");
  fprintf(rk2_vs_rk4, "pause -1");

  fclose(rk2_vs_rk4);
}
