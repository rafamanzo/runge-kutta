#define MAX_POINTS 10000

extern "C" void rk2_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers);
extern "C" void rk4_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers);
