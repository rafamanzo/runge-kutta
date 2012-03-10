#include<stdlib.h>
#include "cuda.h"
#include "cuda_runtime.h"
#include "../first_order_function.h"
#include "rk_kernel.h"

__global__ void rk2_kernel(double *x0, double *y0, double h, fof *dydx, double *y1){
  __shared__ double k1, k2;
  __shared__ int i = threadIdx.x;
  
  k1 = h*(dydx[i].a*x0[i] + dydx[i].b*y0[i] + dydx[i].c);
  k2 = h*(dydx[i].a*(x0[i] + h/2.0) + dydx[i].b*(y0[i] + k1/2.0) + dydx[i].c);
  y1[i] = y0[i] + k2;
}


__global__ void rk4_kernel(double *x0, double *y0, double h, fof *dydx, double *y1){
  __shared__ double k1, k2, k3, k4;
  __shared__ int i = threadIdx.x;
  
  k1 = h*(dydx[i].a*x0[i] + dydx[i].b*y0[i] + dydx[i].c);
  k2 = h*(dydx[i].a*(x0[i] + h/2.0) + dydx[i].b*(y0[i] + k1/2.0) + dydx[i].c);
  k3 = h*(dydx[i].a*(x0[i] + h/2.0) + dydx[i].b*(y0[i] + k2/2.0) + dydx[i].c);
  k4 = h*(dydx[i].a*(x0[i] + h) + dydx[i].b*(y0[i] + k3) + dydx[i].c);
  y1[i] = y0[i] + k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0;
}

void rk2_caller(double *x0, double *y0, int n, double h, fof *dydx, double *y1){
  double *d_x0;
  double *d_y0;
  fof *d_dydx;
  double *d_y1;
  
  cudaMalloc(&d_x0, n*sizeof(double));
  cudaMalloc(&d_y0, n*sizeof(double));
  cudaMalloc(&d_dydx, n*sizeof(fof));
  cudaMalloc(&d_y1, n*sizeof(double));
  
  cudaMemcpy(d_x0, x0, n*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(d_y0, y0, n*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(d_dydx, dydx, n*sizeof(fof), cudaMemcpyHostToDevice);
  
  rk2_kernel<<<1,n>>>(d_x0, d_y0, h, d_dydx, d_y1);
  
  cudaMemcpy(y1, d_y1, n*sizeof(double), cudaMemcpyDeviceToHost);
}

void rk4_caller(double *x0, double *y0, int n, double h, fof *dydx, double *y1){
  double *d_x0;
  double *d_y0;
  fof *d_dydx;
  double *d_y1;
  
  cudaMalloc(&d_x0, n*sizeof(double));
  cudaMalloc(&d_y0, n*sizeof(double));
  cudaMalloc(&d_dydx, n*sizeof(fof));
  cudaMalloc(&d_y1, n*sizeof(double));
  
  cudaMemcpy(d_x0, x0, n*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(d_y0, y0, n*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(d_dydx, dydx, n*sizeof(fof), cudaMemcpyHostToDevice);
  
  rk4_kernel<<<1,n>>>(d_x0, d_y0, h, d_dydx, d_y1);
  
  cudaMemcpy(y1, d_y1, n*sizeof(double), cudaMemcpyDeviceToHost);
}
