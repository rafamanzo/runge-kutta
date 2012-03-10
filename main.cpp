#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "first_order_function.h"
#include "rk.h"
#include "input.h"

int main(int argc, char *argv[]){
  char input[50];
  double *x0, *y0, *y1, *y1rk4, h;
  int n, i;
  fof *dydx;
  
  x0 = y0 = y1 = NULL;
  dydx = NULL;
  
  strcpy(input, argv[1]);
  parseFile(input, &n, &h, &x0, &y0, &dydx);
  y1 = (double *) malloc(n*sizeof(double));
  y1rk4 = (double *) malloc(n*sizeof(double));
  
  rk2(x0,y0,n,h,dydx,y1);
  rk4(x0,y0,n,h,dydx,y1rk4);
  
  for(i = 0; i < n; i++)
    printf("\nRK2: %f | RK4: %f\n", y1[i], y1rk4[i]);
  
  free(x0);
  free(y0);
  free(y1);
  free(y1rk4);
  free(dydx);
  
  return 0;
}
