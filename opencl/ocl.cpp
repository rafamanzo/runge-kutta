#include <stdio.h>
#include <stdlib.h>
#include "ocl.h"

/* Objetos do Open CL */
cl_platform_id platform;
cl_context context;
cl_device_id* devices;
cl_command_queue queue;
cl_kernel kernel;
cl_program program;
cl_event event;
cl_mem points, out;

/* Informacoes sobre os devices */
unsigned int devices_found;
unsigned int device_used = 0;

unsigned int opencl_create_platform(unsigned int num_platforms){
  char name[MAXSTR];
  int num_platforms_found;
  
  clGetPlatformIDs( 0, NULL, &num_platforms_found);
  printf("Num Plat == %d\n\n", num_platforms_found); 
  if ( clGetPlatformIDs( num_platforms, &platform, &num_platforms_found ) == CL_SUCCESS ){
    /* As duas linhas abaixo sao usadas para teste. */
    //clGetPlatformInfo( platform, CL_PLATFORM_NAME, MAXSTR, &name, NULL );
    //printf("Nome da plataforma %s\n",name);  
    return num_platforms_found;
  }
  else
    return -1;
}

unsigned int opencl_get_devices_id(cl_device_type device_type) {
  unsigned int vendor_id;
  
  /* Achando o numero de devices na maquina */
  clGetDeviceIDs(platform, device_type, 0, NULL, &devices_found);
  devices = malloc(devices_found*(sizeof(cl_device_id)));
  
  if(clGetDeviceIDs( platform, device_type, devices_found, devices, NULL) == CL_SUCCESS){
    /* As duas linhas abaixo sao usadas para teste. 
    clGetDeviceInfo( devices, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(unsigned int), &vendor_id, NULL );
    printf("Vendor ID do Device %d\n",vendor_id); */
    return devices_found;
  }
  else
    return -1;
}

int opencl_create_context(){
  if( (context = clCreateContext( 0, 1, devices, NULL, NULL, NULL )) != NULL ){
    return 1;
  }
  else 
    return -1;
}

int opencl_create_queue() {
  if((queue = clCreateCommandQueue(context, devices[device_used], 0, NULL)) != NULL ){
    return 1;
  }
  else 
    return -1;
}

/* Funcoes auxiliares para a criacao do program */
char* loadProgramFromSource(char* program_path, int *size) {
  char* program_string;
  FILE* prog;

  prog = fopen(program_path, "r");
  fseek(prog, 0, SEEK_END);
  *size = ftell(prog);
  fseek(prog, 0, SEEK_SET);

  program_string = malloc((*size+1)*sizeof(char));
  *size = fread(program_string, 1, *size, prog);
  fclose(prog);
  program_string[*size] = '\0';

  return program_string;
}

int buildProgram() {
  int err;
  char *build_log;
  size_t ret_val_size;
                
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if ( err != CL_SUCCESS ) {
    clGetProgramBuildInfo(program, devices[device_used], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);

    build_log = malloc((ret_val_size+1)*sizeof(char));
    clGetProgramBuildInfo(program, devices[device_used], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
    build_log[ret_val_size] = '\0';

    printf("BUILD LOG: \n %s", build_log);
    printf("program built\n");
    return -1;
  }
  else
    return 1;
}
/* Fim das funcoes auxiliares para a criacao do program */

int opencl_create_program(char* program_path){
  char* program_source;
  int size;
  size_t prog_size;
  cl_int err;
  
  program_source = loadProgramFromSource(program_path, &size);
  prog_size = (size_t)size;
  program = clCreateProgramWithSource(context, 1, (const char**)&program_source, &prog_size, &err);
  if ( err != CL_SUCCESS )
    printf("ERROR %d.\n",err);
  return buildProgram();
}

void opencl_create_kernel(char* kernel_name){
  cl_int err;

  kernel = clCreateKernel( program, (const char*) kernel_name, &err);

  if ( kernel == NULL ){
    printf("ERROR: Failed to create kernel %s.\n",kernel_name);
    exit(-1);
  }
}

void prepare_kernel( char *kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points) {

  opencl_create_kernel(kernel_name);
  /* Validar permissoes */
  points = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(vector)*n_x*n_y*n_z, field.vectors, NULL);
  out = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(vector)*n_x*n_y*n_z, NULL, NULL);

  clSetKernelArg(kernel, 0, sizeof(cl_mem), v0);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), count_v0);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), h;
  clSetKernelArg(kernel, 3, sizeof(cl_mem), n_x);
  clSetKernelArg(kernel, 4, sizeof(cl_mem), n_y);
  clSetKernelArg(kernel, 5, sizeof(cl_mem), n_z);
  clSetKernelArg(kernel, 6, sizeof(cl_mem), field);
  clSetKernelArg(kernel, 7, sizeof(cl_mem), points); //validar
  clSetKernelArg(kernel, 8, sizeof(cl_mem), n_points); //validar

  clFinish(queue);
}

void opencl_run_kernel(int n_x, int n_y, int n_z){
  size_t work_dim[1];
  int i;

  work_dim[0] = n_x*n_y*n_z;

  clEnqueueNDRangeKernel(queue, kernel, 1, NULL, work_dim, NULL, 0, NULL, &event);
  clReleaseEvent(event);
  clFinish(queue);

  if( clEnqueueReadBuffer(queue, out, CL_TRUE, 0, n_x*n_y*n_z, &out, 0, NULL, &event) == CL_INVALID_VALUE ) 
	  printf("ERROR: Failed to read buffer.\n");
  clReleaseEvent(event);

  for( i = 0; i < n_x*n_y*n_z; i++ ){
    printf("%f %f %f\n", out[i].x,out[i].y,out[i].z);
}
