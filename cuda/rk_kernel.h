/*************************************************************************/
/* The worst case is a 512x512x512 field                                 */
/* Where the cube diagonal has size 887 aproximating                     */
/* So for a 0.1 step-size we have something like 9000 points             */
/* 50000 is a good extrapolation int this case                           */
/* Each double uses 8 bytes, so for 50000 points we have 0.4MB           */
/* So for 1000 initial points we wil use something like 400MB            */
/* 400MB is nothing nowadays and we are good!                            */
/* Concluding 50000 is a good number and there is some math behind it... */
/*************************************************************************/
#define MAX_POINTS 50000

extern "C" void rk2_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers);
extern "C" void rk4_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers);
