#include<stdlib.h>
#include "../first_order_function.h"
#include "../rk.h"
#include "rk_kernel.h"

void rk2(double *x0, double *y0, int n, double h, fof *dydx, double *y1){
  rk2_caller(x0, y0, n, h, dydx, y1);
}

void rk4(double *x0, double *y0, int n, double h, fof *dydx, double *y1){
  rk4_caller(x0, y0, n, h, dydx, y1);
}
