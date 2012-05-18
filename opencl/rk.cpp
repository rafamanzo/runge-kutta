#include <stdio.h>
#include <stdlib.h>
#include "ocl.h"
#include "../rk.h"

void rk2(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  /*opencl_init();*/
  //printf("Iniciando a criacao do programa...\n");
  /*if ( opencl_create_program((char*)"opencl/rk.cl") <= 0 ) {
    printf("Erro na criacao do programa\n");
    return;
  }
  else printf("Programa criado com sucesso!\n");
  printf("\n");
  prepare_kernel((char*)"rk2_kernel",v0,count_v0,h,n_x,n_y,n_z,field,points,n_points);
  opencl_run_kernel(n_x,n_y,n_z);*/
}

void rk4(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  opencl_init((char*)"rk4_kernel",v0,count_v0,h,n_x,n_y,n_z,field,points,n_points);
}
