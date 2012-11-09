#define MAX_POINTS 10000
#include <opcl.h>

void opencl_init(char* kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers);
