#include "CL/opencl.h"

unsigned int opencl_create_platform(unsigned int num_platforms);
unsigned int opencl_get_devices_id(cl_device_type device_type);
int opencl_create_context();
int opencl_create_queue();
int opencl_create_program(char* program_path);
int opencl_run_kernel();
