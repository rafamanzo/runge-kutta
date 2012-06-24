#include<stdlib.h>
#include<stdio.h>
#include "dataset.h"
#include "fiber.h"
#include "output.h"

using namespace runge_kutta;

void generate_gnuplot_input(DataSet dataset, unsigned v0_count, Fiber *rk2_fibers, Fiber *rk4_fibers){
  FILE *rk2_dat, *rk4_dat, *rk2_vs_rk4;
  unsigned i,k;
  
  rk2_dat = fopen("rk2.dat", "w");
  rk4_dat = fopen("rk4.dat", "w");
  rk2_vs_rk4 = fopen("rk2-vs-rk4.p", "w");
  
  for(k = 0; k < v0_count; k++){
    for(i = 0; i < rk2_fibers[k].pointsCount(); i++)
      fprintf(rk2_dat, "%f %f %f\n", rk2_fibers[k].getPoint(i).x, rk2_fibers[k].getPoint(i).y, rk2_fibers[k].getPoint(i).z);
    fprintf(rk2_dat, "\n");
    
    for(i = 0; i < rk4_fibers[k].pointsCount(); i++)
      fprintf(rk4_dat, "%f %f %f\n", rk4_fibers[k].getPoint(i).x, rk4_fibers[k].getPoint(i).y, rk4_fibers[k].getPoint(i).z);
    fprintf(rk4_dat, "\n");
  }
  
  fclose(rk2_dat);
  fclose(rk4_dat);
  
  fprintf(rk2_vs_rk4, "set xrange[0:%d]\n", dataset.n_x() - 1);
  fprintf(rk2_vs_rk4, "set yrange[0:%d]\n", dataset.n_y() - 1);
  fprintf(rk2_vs_rk4, "set zrange[0:%d]\n", dataset.n_z() - 1);
  fprintf(rk2_vs_rk4, "splot \"rk2.dat\" w lines lt 1, \"rk4.dat\" w lines lt 2\n");
  fprintf(rk2_vs_rk4, "pause -1");

  fclose(rk2_vs_rk4);
}
