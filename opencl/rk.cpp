#include <stdio.h>
#include<stdlib.h>
#include "ocl.h"
#include "../vector_field.h"
#include "../rk.h"
//#include "rk_kernel.h"

/* deve ir para ocl.ccp e receber kernel_name*/
int opencl_init(){
  unsigned int num_platforms, num_devices;

  printf("\n");
  printf("Iniciando a Camada de Plataforma do OpenCL...\n");
  if ( ( num_platforms =  opencl_create_platform(2)  ) <= 0 ) {
      printf("Erro na criacao da camada de plataforma!\n");
      return -1; 
  }
  else printf("Num Plataformas = %d\n", num_platforms);
  printf("\n");
  
  printf("Iniciando a Busca por Devices...\n");
  if ( ( num_devices = opencl_get_devices_id(CL_DEVICE_TYPE_GPU) ) <= 0 ) {
    printf("Erro na busca por devices!\n");
    return -1;
  }
  else printf("Num devices = %d\n", num_devices);
  printf("\n");

  printf("Iniciando a criacao do contexto...\n");
  if ( opencl_create_context() <= 0 ) {
    printf("Erro na criacao do contexto!\n");
    return -1;
  }
  else printf("Contexto criado com sucesso!\n");
  printf("\n");

  printf("Iniciando a criacao da fila de comandos...\n");
  if ( opencl_create_queue() <= 0 ) {
    printf("Erro na criacao da fila de comandos\n");
    return -1;
  }
  else printf("Fila de comandos criado com sucesso!\n");
  printf("\n");

  printf("Iniciando a criacao do programa...\n");
  if ( opencl_create_program("matrixmulti.cl") <= 0 ) {
    printf("Erro na criacao do programa\n");
    return -1;
  }
  else printf("Programa criado com sucesso!\n");
  printf("\n");

  return 1;
}


void rk2(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  opencl_init();
  prepare_kernel("rk2_kernel",v0,count_v0,h,n_x,n_y,n_z,field,points,n_points);
  opencl_run_kernel(saida);//definir arquivo de saida
}

void rk4(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  opencl_init();
  prepare_kernel("rk4_kernel",v0,count_v0,h,n_x,n_y,n_z,field,points,n_points);
  opencl_run_kernel();//definir arquivo de saida

}
