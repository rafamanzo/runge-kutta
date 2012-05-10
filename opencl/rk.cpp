#include <stdio.h>
#include "opcl.h"

int iniciar_opencl() {
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

  printf("Iniciando a criacao do kernel...\n");
  if ( opencl_create_kernel("matrixmulti") <= 0 ) {
    printf("Erro na criacao do kernel\n");
    return -1;

  else printf("kernel criado com sucesso!\n");
  printf("\n");
  return 1;
}

int main() {
  
  if ( iniciar_opencl() == 1 ) {
    printf("Platafoma do OpenCL criada com sucesso, iniciando a multiplica����o das matrizes...\n");
    opencl_run_kernel();
  }
  return 0;
}

