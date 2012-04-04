#include<stdlib.h>
#include<math.h>
#include "cuda.h"
#include "cuda_runtime.h"
#include "../vector_field.h"
#include "vector_operations.h"
#include "rk_kernel.h"

__device__ vector nearest_neighbour(vector v0, int n_x, int n_y, int n_z, vector_field field){
  int x, y, z;
  vector zero;
  
  zero.x = zero.y = zero.z = 0.0;
  
  if( (v0.x - floor(v0.x)) > 0.5 && v0.x < (n_x - 1))
    x = (int) ceil(v0.x);
  else
    x = (int) floor(v0.x);
    
  if( (v0.y - floor(v0.y)) > 0.5 && v0.y < (n_y - 1))
    y = (int) ceil(v0.y);
  else
    y = (int) floor(v0.y);
    
  if( (v0.z - floor(v0.z)) > 0.5 && v0.z < (n_z - 1))
    z = (int) ceil(v0.z);
  else
    z = (int) floor(v0.z);

  if(x >= n_x || y >= n_y || z >= n_z || x < 0 || y < 0 || z < 0){
    return zero;
  }else{
    return field[offset(n_x, n_y, x, y, z)];
  }
}

__device__ vector trilinear_interpolation(vector v0, int n_x, int n_y, int n_z, vector_field field){
  int x1, y1, z1, x0, y0, z0, xd, yd, zd;
  vector zero, i1, i2, j1, j2, w1, w2;
  
  zero.x = zero.y = zero.z = 0.0;
  
  x1 = ceil(v0.x);
  y1 = ceil(v0.y);
  z1 = ceil(v0.z);
  x0 = floor(v0.x);
  y0 = floor(v0.y);
  z0 = floor(v0.z);
  
  if(x1 >= n_x || y1 >= n_y || z1 >= n_z || x0 < 0 || y0 < 0 || z0 < 0){
    return nearest_neighbour(v0, n_x, n_y, n_z, field);
  }else{
    xd = (v0.x - x0)/(x1 - x0);
    yd = (v0.y - y0)/(y1 - y0);
    zd = (v0.z - z0)/(z1 - z0);
    
    set(&i1, sum( mult_scalar(field[offset(n_x, n_y, x0, y0, z0)], (1.0 - zd)), mult_scalar(field[offset(n_x, n_y, x0, y0, z1)], zd) ) );
    set(&i2, sum( mult_scalar(field[offset(n_x, n_y, x0, y1, z0)], (1.0 - zd)), mult_scalar(field[offset(n_x, n_y, x0, y1, z1)], zd) ) );
    set(&j1, sum( mult_scalar(field[offset(n_x, n_y, x1, y0, z0)], (1.0 - zd)), mult_scalar(field[offset(n_x, n_y, x1, y0, z1)], zd) ) );
    set(&j2, sum( mult_scalar(field[offset(n_x, n_y, x1, y1, z0)], (1.0 - zd)), mult_scalar(field[offset(n_x, n_y, x1, y1, z1)], zd) ) );
    
    set(&w1, sum( mult_scalar(i1, (1.0 - yd)), mult_scalar(i2, yd) ) );
    set(&w2, sum( mult_scalar(j1, (1.0 - yd)), mult_scalar(j2, yd) ) );
    
    return sum( mult_scalar(w1, (1.0 - xd)), mult_scalar(w2, xd) );
  }
}

__global__ void rk2_kernel(double *x0, double *y0, double h, fof *dydx, double *y1){
  __shared__ double k1, k2;
  __shared__ int i;

  i = threadIdx.x;
  
  k1 = h*(dydx[i].a*x0[i] + dydx[i].b*y0[i] + dydx[i].c);
  k2 = h*(dydx[i].a*(x0[i] + h/2.0) + dydx[i].b*(y0[i] + k1/2.0) + dydx[i].c);
  y1[i] = y0[i] + k2;
}


/*__global__ void rk4_kernel(double *x0, double *y0, double h, fof *dydx, double *y1){
  __shared__ double k1, k2, k3, k4;
  __shared__ int i;
  
  i = threadIdx.x;
  
  k1 = h*(dydx[i].a*x0[i] + dydx[i].b*y0[i] + dydx[i].c);
  k2 = h*(dydx[i].a*(x0[i] + h/2.0) + dydx[i].b*(y0[i] + k1/2.0) + dydx[i].c);
  k3 = h*(dydx[i].a*(x0[i] + h/2.0) + dydx[i].b*(y0[i] + k2/2.0) + dydx[i].c);
  k4 = h*(dydx[i].a*(x0[i] + h) + dydx[i].b*(y0[i] + k3) + dydx[i].c);
  y1[i] = y0[i] + k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0;
}*/

void rk2_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  vector *d_v0;
  vector_field d_field;
  vector **d_points;
  int *d_n_points;
  
  cudaMalloc(&d_v0, count_v0*sizeof(vector));
  cudaMalloc(&d_field, n_x*n_y*n_z*sizeof(vector));
  cudaMalloc(&d_points, count_v0*sizeof(vector *));
  cudaMalloc(&d_n_points, count_v0*sizeof(int));
  
  cudaMemcpy(d_v0, v0, count_v0*sizeof(vector), cudaMemcpyHostToDevice);
  cudaMemcpy(d_field, field, n_x*n_y*n_z*sizeof(vector), cudaMemcpyHostToDevice);
  
  rk2_kernel<<<1,n>>>(d_v0, count_v0, h, n_x, n_y, n_z, d_field, d_points, d_n_points);
  
  cudaMemcpy(y1, d_y1, n*sizeof(double), cudaMemcpyDeviceToHost);
  
  cudaFree(d_x0);
  cudaFree(d_y0);
  cudaFree(d_y1);
  cudaFree(d_dydx);
}

/*void rk4_caller(double *x0, double *y0, int n, double h, fof *dydx, double *y1){
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
  
  cudaFree(d_x0);
  cudaFree(d_y0);
  cudaFree(d_y1);
  cudaFree(d_dydx);
}*/
