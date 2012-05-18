#include <CL/cl.h>
#include "../vector_field.h"

unsigned int opencl_create_platform(unsigned int num_platforms);
unsigned int opencl_get_devices_id(cl_device_type device_type);
int opencl_create_context();
int opencl_create_queue();
int opencl_create_program(char* program_path);
int opencl_run_kernel();
void prepare_kernel( char *kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points);
