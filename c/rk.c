#include<stdlib.h>
#include "../rk.h"

void rk2(double *x0, double *y0, int n, double h, fof *dydx, double *y1){
  double k1, k2;
  int i;
  
  for(i = 0; i < n; i++){
    k1 = h*(dydx[i].a*x0[i] + dydx[i].b*y0[i] + dydx[i].c);
    k2 = h*(dydx[i].a*(x0[i] + h/2.0) + dydx[i].b*(y0[i] + k1/2.0) + dydx[i].c);
    y1[i] = y0[i] + k2;
  }
}
