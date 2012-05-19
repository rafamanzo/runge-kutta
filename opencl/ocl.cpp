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
cl_mem v_initial, fld, pts, n_pts;
/* Informacoes sobre os devices */
unsigned int devices_found;
unsigned int device_used = 0;

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


void prepare_kernel( char *kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points) {
  cl_int err;

  opencl_create_kernel(kernel_name);
  v_initial = clCreateBuffer(context, CL_MEM_READ_ONLY| CL_MEM_COPY_HOST_PTR, sizeof(vector)*count_v0,(void*)&v0, NULL); 
  fld = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(vector)*n_x*n_y*n_z,(void*)&field, &err);
  pts = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float3)*n_x*n_y*n_z, NULL, NULL);
  n_pts = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float3)*count_v0, NULL, NULL);

printf("Antes!\n");

  if(v_initial == NULL)
    printf("v_initial is NULL\n");
  if(fld == NULL)
    printf("fld is NULL err = %d\n",err);
  if( err == CL_OUT_OF_HOST_MEMORY)
    printf("CL_OUT_OF_HOST_MEMORY\n");
  if(pts == NULL)
    printf("pts is NULL\n");
  if(n_pts == NULL)
    printf("n_pts is NULL\n");

  /*n_pts = (cl_float3*) malloc(sizeof(cl_float3)*count_v0);
  for( i = 0; i < count_v0; i++){
    n_pts[i].x = (float)v0[i].x;
    n_pts[i].y = (float)v0[i].y;
    n_pts[i].z = (float)v0[i].z;
  }
    
  entrada = (cl_float3*) malloc(sizeof(cl_float3)*n_x*n_y*n_z);
  for( i = 0; i < count_v0; i++){
    entrada[i].x = (float)field[i].x;
    entrada[i].y = (float)field[i].y;
    entrada[i].z = (float)field[i].z;
  }*/
  if( clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&v_initial) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 0.\n");
  if( clSetKernelArg(kernel, 1, sizeof(int), (void*)&count_v0) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 1.\n");
  if( clSetKernelArg(kernel, 2, sizeof(double), (void*)&h) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 2.\n");
  if( clSetKernelArg(kernel, 3, sizeof(int), (void*)&n_x) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 3.\n");
  if( clSetKernelArg(kernel, 4, sizeof(int), (void*)&n_y) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 4.\n");
  if( clSetKernelArg(kernel, 5, sizeof(int), (void*)&n_z) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 5.\n");
  if( clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&fld) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 6.\n");
  if( clSetKernelArg(kernel, 7, sizeof(cl_mem), (void*)&pts) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 7.\n");
  if( clSetKernelArg(kernel, 8, sizeof(cl_mem), (void*)&n_pts) != CL_SUCCESS)
    printf("ERROR: Failed to set kernel argument 8.\n");
printf("Depois!\n");
  clFinish(queue);//colocar na fila?!
}

void opencl_run_kernel(int n_x, int n_y, int n_z, int count_v0){
  size_t work_dim[1];
  int i; cl_int err;

  work_dim[0] = n_x*n_y*n_z;

  clEnqueueNDRangeKernel(queue, kernel, 1, NULL, work_dim, NULL, 0, NULL, &event);
  clReleaseEvent(event);
  clFinish(queue);
printf("INSIDE\n");
  //validar funcao abaixo
  if( (err = clEnqueueReadBuffer(queue, pts, CL_TRUE, 0, sizeof(vector)*n_x*n_y*n_z, &pts, 0, NULL, &event)) == CL_INVALID_VALUE ){ 
	  printf("\nERROR: Failed to read buffer pts %d.\n",err);
    exit(-1);
  }
  if( (err = clEnqueueReadBuffer(queue, n_pts, CL_TRUE, 0, sizeof(vector)*count_v0, &n_pts, 0, NULL, &event)) == CL_INVALID_VALUE ){ 
	  printf("\nERROR: Failed to read buffer n_points %d.\n",err);
    exit(-1);
  }

  clReleaseEvent(event);

 //for( i = 0; i < n_x*n_y*n_z; i++ )
   //printf("%f %f %f\n", out[i].x,out[i].y,out[i].z);
}

void opencl_init(char* kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  unsigned int num_platforms, num_devices;

  printf("Starting OpenCL platform...");
  num_platforms =  opencl_create_platform(2); 
   
  printf("Searching for devices...\n");
  num_devices = opencl_get_devices_id(CL_DEVICE_TYPE_GPU);
  printf(" Num devices = %d OK.\n", num_devices);

  printf("Creating context...");
  opencl_create_context();
  printf(" OK.\n");
    
  printf("Creating queue...");
  opencl_create_queue();
  printf(" OK.\n");
  
  printf("Creating program...");
  opencl_create_program((char*)"opencl/rk.cl");
  printf(" OK.\n");

  printf("Building program...");
  opencl_build_program();
  printf(" OK.\n");
  
  printf("Preparing the kernel...");
  prepare_kernel((char*)"rk4_kernel",v0,count_v0,h,n_x,n_y,n_z,field,points,n_points);
  printf(" OK.\n");

  printf("Running the kernel...");
  opencl_run_kernel(n_x,n_y,n_z, count_v0);
  printf(" OK.\n");
}
