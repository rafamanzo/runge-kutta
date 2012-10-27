
#include <stdio.h>
#include <dataset.h>
#include <fiber.h>
#include <opcl.h>

using namespace runge_kutta;

RK_OpenCL::RK_OpenCL(){
  _device_used = 0;
}

unsigned int RK_OpenCL::opencl_create_platform(unsigned int num_platforms){
  int num_platforms_found;
  
  clGetPlatformIDs( 0, NULL, (cl_uint*)&num_platforms_found);
  if ( clGetPlatformIDs( num_platforms, &_platform, (cl_uint*)&num_platforms_found ) != CL_SUCCESS ){
    printf("\nERROR: Failed to create plataform.\n");
    exit(-1);
  }
  return num_platforms_found;
}

unsigned int RK_OpenCL::opencl_get_devices_id(cl_device_type device_type) {
  clGetDeviceIDs(_platform, device_type, 0, NULL, &_devices_found);
  _devices = (cl_device_id*) malloc(_devices_found*(sizeof(cl_device_id)));
  
  if(clGetDeviceIDs( _platform, device_type, _devices_found, _devices, NULL) != CL_SUCCESS){
    printf("\nERROR: Failed to get devices id's.\n");
    exit(-1);
  }
  return _devices_found;
}

void RK_OpenCL::opencl_create_context(){
  if( (_context = clCreateContext( 0, 1, _devices, NULL, NULL, NULL )) == NULL ){
    printf("\nERROR: Failed to create context.\n");
    exit(-1);
  }
}

void RK_OpenCL::opencl_create_queue(){
  if((_queue = clCreateCommandQueue(_context, _devices[_device_used], 0, NULL)) == NULL ){
    printf("\nERROR: Failed to create queue.\n");
    exit(-1);
  }
}

/* Funções auxiliares para a criação do programa */
char* RK_OpenCL::load_program_from_source(int *size) {
  char* program_string;
  FILE* prog;

  prog = fopen("core/opencl/rk_kernel.cl", "r");
  fseek(prog, 0, SEEK_END);
  *size = ftell(prog);

  fseek(prog, 0, SEEK_SET);

  program_string = (char*) malloc((*size+1)*sizeof(char));
  *size = fread(program_string, 1, *size, prog);
  fclose(prog);
  program_string[*size] = '\0';

  return program_string;
}

void RK_OpenCL::opencl_build_program(){
  int err;
  char *build_log;
  size_t ret_val_size;
                
  err = clBuildProgram(_program, 0, NULL, NULL, NULL, NULL);
  if ( err != CL_SUCCESS ) {
    printf("\nERROR: Failed to build program.\n");
    clGetProgramBuildInfo(_program, _devices[_device_used], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);

    build_log = (char*) malloc((ret_val_size+1)*sizeof(char));
    clGetProgramBuildInfo(_program, _devices[_device_used], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
    build_log[ret_val_size] = '\0';
    printf("BUILD LOG: \n %s\n", build_log);
    exit(-1);
  }
}
/* Fim das funções auxiliares para a criação do program */

void RK_OpenCL::opencl_create_program(){
  char* program_source;
  int size;
  size_t prog_size;
  cl_int err;

  program_source = load_program_from_source(&size);
  prog_size = (size_t)size;
  _program = clCreateProgramWithSource(_context, 1, (const char**)&program_source, &prog_size, &err);

  if ( err != CL_SUCCESS ){
    printf("\nERROR: Failed to create program.\n");
    exit(-1);
  }
  opencl_build_program();
}

void RK_OpenCL::opencl_create_kernel(char* kernel_name){
  cl_int err;

  _kernel = clCreateKernel(_program, kernel_name, &err);
  if ( _kernel == NULL ){
    printf("\nERROR: Failed to create kernel %s.\n",kernel_name);
    exit(-1);
  }
}

void RK_OpenCL::prepare_kernel(vector *v0, int count_v0, TYPE h, int n_x,int n_y,int n_z, vector_field field, vector *points, int max_points){
  cl_mem opencl_count_v0, opencl_h, opencl_n_x, opencl_n_y, opencl_n_z, opencl_max_points;

  /* Criação dos buffers que o OpenCL vai usar. */
  _opencl_v0 = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(vector)*count_v0, v0, NULL);
  opencl_count_v0 = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (&count_v0), NULL);
  opencl_h = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(TYPE), (&h), NULL);
  opencl_n_x = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (&n_x), NULL);
  opencl_n_y = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (&n_y), NULL);
  opencl_n_z = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (&n_z), NULL);  
  _opencl_field = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(field)*n_x*n_y*n_z, field, NULL);
  _opencl_points = clCreateBuffer(_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(vector)*count_v0*max_points, points, NULL);
  _opencl_n_points = clCreateBuffer(_context, CL_MEM_READ_ONLY, sizeof(int)*count_v0, NULL,NULL);
  opencl_max_points = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (&max_points), NULL);

  clSetKernelArg(_kernel, 0, sizeof(cl_mem), (void *)&_opencl_v0);
  clSetKernelArg(_kernel, 1, sizeof(cl_mem), (void *)&opencl_count_v0);
  clSetKernelArg(_kernel, 2, sizeof(cl_mem), (void *)&opencl_h);
  clSetKernelArg(_kernel, 3, sizeof(cl_mem), (void *)&opencl_n_x);
  clSetKernelArg(_kernel, 4, sizeof(cl_mem), (void *)&opencl_n_y);
  clSetKernelArg(_kernel, 5, sizeof(cl_mem), (void *)&opencl_n_z);
  clSetKernelArg(_kernel, 6, sizeof(cl_mem), (void *)&_opencl_field);
  clSetKernelArg(_kernel, 7, sizeof(cl_mem), (void *)&_opencl_points);
  clSetKernelArg(_kernel, 8, sizeof(cl_mem), (void *)&_opencl_n_points);
  clSetKernelArg(_kernel, 9, sizeof(cl_mem), (void *)&opencl_max_points);

  clFinish(_queue);
}

void RK_OpenCL::opencl_run_kernel(vector *points, int *n_points, int max_points, int count_v0){
  size_t work_dim[1];
  
  work_dim[0] = max_points;
  clEnqueueNDRangeKernel(_queue, _kernel, 1, NULL, work_dim, NULL, 0, NULL, &_event);
  clReleaseEvent(_event);
  clFinish(_queue);

  if( clEnqueueReadBuffer(_queue, _opencl_n_points, CL_TRUE, 0, sizeof(TYPE)*count_v0, n_points, 0, NULL, &_event) == CL_INVALID_VALUE ){
    printf("\nERROR: Failed to read buffer \"n_points\".\n");
    exit(-1);
  }
  if( clEnqueueReadBuffer(_queue, _opencl_points, CL_TRUE, 0, sizeof(TYPE)*max_points, points, 0, NULL, &_event) == CL_INVALID_VALUE ){
    printf("\nERROR: Failed to read buffer \"points\".\n");
    exit(-1);
  }
  clReleaseEvent(_event);
}

void RK_OpenCL::opencl_init(char* kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers){
  unsigned int num_platforms, num_devices;
  int max_points, available;
  vector *points;
  int *n_points, i, j; 

  printf("Starting OpenCL platform...");
  num_platforms =  opencl_create_platform(2); 
  printf(" Num Platforms = %d OK.\n", num_platforms);
   
  printf("Searching for devices...");
  num_devices = opencl_get_devices_id(CL_DEVICE_TYPE_GPU);
  printf(" Num devices = %d OK.\n", num_devices);

  printf("Creating context...");
  opencl_create_context();
  printf(" OK.\n");
    
  printf("Creating queue...");
  opencl_create_queue();
  printf(" OK.\n");
  
  printf("Creating program...");
  opencl_create_program();
  printf(" OK.\n");

  printf("Creating kernel...");
  opencl_create_kernel(kernel_name);
  printf(" OK.\n");

  printf("Memory allocation...");
  n_points = (int*) malloc(count_v0*sizeof(int));
  clGetDeviceInfo(*_devices,CL_DEVICE_GLOBAL_MEM_SIZE,sizeof(available),&available,NULL);
  max_points = (available*0.9)/(sizeof(vector)*count_v0);
  points = (vector*) malloc(count_v0*max_points*sizeof(vector));
  printf(" OK.\n");

  printf("Preparing the kernel...");
  prepare_kernel(v0,count_v0, h, n_x, n_y, n_z, field, points, max_points);
  printf(" OK.\n");

  printf("Running the kernel...");
  opencl_run_kernel(points,n_points,max_points,count_v0);
  printf(" OK.\n");fflush(NULL);

  printf("Coping the result...");
  *fibers = (runge_kutta::Fiber *) malloc(count_v0*sizeof(runge_kutta::Fiber));
  for(i = 0; i < count_v0; i++){
    (*fibers)[i] = runge_kutta::Fiber(n_points[i]);
    for(j = 0; j < n_points[i]; j++){
      (*fibers)[i].setPoint(j, points[(i+count_v0*j)]);
    }
  }
  printf(" OK.\n");
  
  free(n_points);
  free(points);  
}

void RK_OpenCL::rk2_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers){
  opencl_init((char*) "rk2_kernel", v0, count_v0, h, n_x, n_y, n_z, field, fibers);
}

void RK_OpenCL::rk4_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers){
  opencl_init((char*) "rk4_kernel", v0, count_v0, h, n_x, n_y, n_z, field, fibers);
}

