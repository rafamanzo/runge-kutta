#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "rk.h"
#include "input.h"

int main(int argc, char *argv[]){
  char input[50];
  double *x0, *y0, *y1, h;
  int n;
  fof *dydx;
  
  x0 = y0 = y1 = NULL;
  dydx = NULL;
  
  strcpy(input, argv[1]);
  parseFile(input, &n, &h, &x0, &y0, &dydx);
  y1 = malloc(n*sizeof(double));

  rk2(x0,y0,n,h,dydx,y1);
  printf("\nRK2: %f\n", y1[0]);
  
  free(x0);
  free(y0);
  free(y1);
  free(dydx);
  
  return 0;
}
