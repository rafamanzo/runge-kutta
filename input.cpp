#include<stdlib.h>
#include<stdio.h>
#include "first_order_function.h"
#include "input.h"

void parseFile(char *file, int *n, double *h, double **x0, double **y0, fof **dydx){
  FILE *fp;
  double *x0_aux, *y0_aux;
  fof *dydx_aux;
  int i;
  
  fp = fopen(file, "r");
  
  fscanf(fp, "%d", n);
  fscanf(fp, "%lf", h); 
  
  x0_aux = (double *) malloc((*n)*sizeof(double));
  y0_aux = (double *) malloc((*n)*sizeof(double));
  dydx_aux = (fof *) malloc((*n)*sizeof(fof));
  
  for(i = 0; i < (*n); i++){
    fscanf(fp, "%lf", &x0_aux[i]);
    fscanf(fp, "%lf", &y0_aux[i]);
    fscanf(fp, "%lf", &dydx_aux[i].a);
    fscanf(fp, "%lf", &dydx_aux[i].b);
    fscanf(fp, "%lf", &dydx_aux[i].c);
  }
  
  *x0 = x0_aux;
  *y0 = y0_aux;
  *dydx = dydx_aux;
  
  fclose(fp);
}
