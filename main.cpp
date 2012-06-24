#include<cstdio>
#include<cstdlib>
#include<cstring>
#include "dataset.h"
#include "rk.h"
#include "input.h"
#include "output.h"

using namespace runge_kutta;

int main(int argc, char *argv[]){
  int v0_count, i;
  vector *v0;
  double h;
  vector **points_rk4, **points_rk2;
  int *n_points_rk4, *n_points_rk2;
  
  Input file = Input(argv[1]);
  DataSet dataset = file.parse(&h, &v0, &v0_count);
  
  RungeKutta rk = RungeKutta(dataset, v0, v0_count, h);
  
  rk.order4(&points_rk4, &n_points_rk4);
  rk.order2(&points_rk2, &n_points_rk2);
  
  generate_gnuplot_input((int) dataset.n_x(), (int) dataset.n_y(), (int) dataset.n_z(), v0_count, n_points_rk2, points_rk2, n_points_rk4, points_rk4);

  free(v0);
  free(n_points_rk4);
  free(n_points_rk2);
  for(i = 0; i < v0_count; i++){
    free(points_rk4[i]);
    free(points_rk2[i]);
  }

  return 0;
}
