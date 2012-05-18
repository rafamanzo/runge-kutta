#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "ocl.h"

/* Objetos do Open CL */
cl_platform_id platform;
cl_context context;
cl_device_id* devices;
cl_command_queue queue;
cl_kernel kernel;
cl_program program;
cl_event event;
cl_mem in, out;

/* Informacoes sobre os devices */
unsigned int devices_found;
unsigned int device_used = 0;


void test_err(cl_int err){
  
  switch(err){
    case CL_INVALID_PROGRAM:
      printf("CL_INVALID_PROGRAM\n");
      break;
    case CL_INVALID_VALUE:
      printf("CL_INVALID_VALUE\n");
      break;
    case CL_INVALID_DEVICE:
      printf("CL_INVALID_DEVICE\n");
      break;
    case CL_INVALID_BINARY:
      printf("CL_INVALID_BINARY\n");
      break;
    case CL_INVALID_BUILD_OPTIONS:
      printf("CL_INVALID_BUILD_OPTIONS\n");
      break;
    case CL_INVALID_OPERATION:
      printf("CL_INVALID_OPERATION\n");
      break;
    case CL_COMPILER_NOT_AVAILABLE:
      printf("CL_COMPILER_NOT_AVAILABLE\n");
      break;
    case CL_BUILD_PROGRAM_FAILURE:
      printf("CL_BUILD_PROGRAM_FAILURE\n");
      break;
    case CL_OUT_OF_HOST_MEMORY:
      printf("CL_OUT_OF_HOST_MEMORY\n");
      break;
    default:
      printf("Unknow error\n");
      break;
  }
}

unsigned int opencl_create_platform(unsigned int num_platforms){
  char name[512];
  int num_platforms_found;
  
  clGetPlatformIDs( 0, NULL, (cl_uint*)&num_platforms_found);
  if ( clGetPlatformIDs( num_platforms, &platform, (cl_uint*)&num_platforms_found ) != CL_SUCCESS ){
    printf("\nERROR: Failed to create plataform.\n");
    exit(-1);
  }
  return num_platforms_found;
}

unsigned int opencl_get_devices_id(cl_device_type device_type) {
  unsigned int vendor_id;
  
  clGetDeviceIDs(platform, device_type, 0, NULL, &devices_found);
  devices = (cl_device_id*) malloc(devices_found*(sizeof(cl_device_id)));
  
  if(clGetDeviceIDs( platform, device_type, devices_found, devices, NULL) != CL_SUCCESS){
    printf("\nERROR: Failed to get devices id's.\n");
    exit(-1);
  }
  return devices_found;
}

void opencl_create_context(){
  if( (context = clCreateContext( 0, 1, devices, NULL, NULL, NULL )) == NULL ){
    printf("\nERROR: Failed to create context.\n");
    exit(-1);
  }
}

void opencl_create_queue(){
  if((queue = clCreateCommandQueue(context, devices[device_used], 0, NULL)) == NULL ){
    printf("\nERROR: Failed to create queue.\n");
    exit(-1);
  }
}

char* load_program_from_source(char* program_path, int *size) {
  char* program_string;
  FILE* prog;

  prog = fopen(program_path, "r");
  fseek(prog, 0, SEEK_END);
  *size = ftell(prog);
  fseek(prog, 0, SEEK_SET);

  program_string = (char*) malloc((*size+1)*sizeof(char));
  *size = fread(program_string, 1, *size, prog);
  fclose(prog);
  program_string[*size] = '\0';

  return program_string;
}

void opencl_build_program(){
  int err;
  char *build_log;
  size_t ret_val_size;
                
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if ( err != CL_SUCCESS ) {
    printf("\nERROR: Failed to build program.\n");
    test_err(err); /* RETIRAR */
    clGetProgramBuildInfo(program, devices[device_used], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);

    build_log = (char*) malloc((ret_val_size+1)*sizeof(char));
    clGetProgramBuildInfo(program, devices[device_used], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
    build_log[ret_val_size] = '\0';
    printf("BUILD LOG: \n %s\n", build_log);
    exit(-1);
  }
}
/* Fim das funcoes auxiliares para a criacao do program */

void opencl_create_program(char* program_path){
  char* program_source;
  int size;
  size_t prog_size;
  cl_int err;
  
  program_source = load_program_from_source(program_path, &size);
  prog_size = (size_t)size;
  program = clCreateProgramWithSource(context, 1, (const char**)&program_source, &prog_size, &err);
  if ( err != CL_SUCCESS ){
    printf("\nERROR: Failed to create program.\n");
    exit(-1);
  }
}

void opencl_create_kernel(char* kernel_name){
  cl_int err;

  kernel = clCreateKernel( program, "rk4_kernel", &err);
  if ( kernel == NULL ){
    printf("\nERROR: Failed to create kernel %s.\n",kernel_name);
    exit(-1);
  }
}
/*int opencl_create_kernel(char* kernel_name) {
  cl_int err;
  kernel = clCreateKernel( program, (const char*) kernel_name, &err);
  if ( err == CL_SUCCESS ) return 1;
  else return -1;
}*/

void prepare_kernel( char *kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points) {

  opencl_create_kernel(kernel_name);
  in = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(vector)*n_x*n_y*n_z,(void*)&field, NULL);
  out = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(vector)*n_x*n_y*n_z, NULL, NULL);

  clSetKernelArg(kernel, 0, sizeof(cl_mem), v0);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&count_v0);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&h);
  clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&n_x);
  clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&n_y);
  clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&n_z);
  clSetKernelArg(kernel, 6, sizeof(cl_mem), in);
  clSetKernelArg(kernel, 7, sizeof(cl_mem), out); 
  clSetKernelArg(kernel, 8, sizeof(cl_mem), n_points);

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
	  printf("\nERROR: Failed to read buffer.\n");
  clReleaseEvent(event);

  //for( i = 0; i < n_x*n_y*n_z; i++ )
  //  printf("%f %f %f\n", out[i].x,out[i].y,out[i].z);
}

void opencl_init(char* kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  unsigned int num_platforms, num_devices;

  printf("Starting OpenCL platform...");
  num_platforms =  opencl_create_platform(2); 
  printf(" Num Plataformas = %d OK.\n", num_platforms);
   
  printf("Starting search for devices...\n");
  num_devices = opencl_get_devices_id(CL_DEVICE_TYPE_GPU);
  printf(" Num devices = %d OK.\n", num_devices);

  printf("Starting context creation...");
  opencl_create_context();
  printf(" OK.\n");
    
  printf("Starting queue creation...");
  opencl_create_queue();
  printf(" OK.\n");
  
  printf("Starting program creation...");
  opencl_create_program((char*)"opencl/rk.cl");
  printf(" OK.\n");

  printf("Start building the program...");
  opencl_build_program();
  printf(" OK.\n");
  
  printf("Start preparing the kernel...");
  prepare_kernel((char*)"rk4_kernel",v0,count_v0,h,n_x,n_y,n_z,field,points,n_points);
  printf(" OK.\n");

  printf("Start running the kernel...");
  opencl_run_kernel(n_x,n_y,n_z);
  printf(" OK.\n");
}
