#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "cuda.h"
#include "cuda_runtime.h"
#include <dataset.h>
#include <fiber.h>
#include <rk_cuda_kernel.h>

/******************************/
/* Auxiliary Vector Functions */
/******************************/

/*FIXME: there must be libraries inside CUDA to work with vectors*/

__device__ int cuda_offset(int n_x, int n_y, int x, int y, int z){
  return x + n_x*y + n_y*n_x*z;
}

__device__ vector sum(vector v1, vector v2){
  vector sum;
  
  sum.x = v1.x + v2.x;
  sum.y = v1.y + v2.y;
  sum.z = v1.z + v2.z;
  
  return sum;
}

__device__ vector subtract(vector v1, vector v2){
  vector subtraction;
  
  subtraction.x = v1.x - v2.x;
  subtraction.y = v1.y - v2.y;
  subtraction.z = v1.z - v2.z;
  
  return subtraction;
}


__device__ vector mult_scalar(vector v, double scalar){
  vector mult;
  
  mult.x = v.x*scalar;
  mult.y = v.y*scalar;
  mult.z = v.z*scalar;
  
  return mult;
}

__device__ void set(vector *x, vector y){
  (*x).x = y.x;
  (*x).y = y.y;
  (*x).z = y.z;
}

__device__ double module(vector v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

__device__ double distance(vector x, vector y){
  return module(sum(x, mult_scalar(y, -1.0)));
}

/************************************/
/* Auxiliary Aproximation Functions */
/************************************/

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
    return field[cuda_offset(n_x, n_y, x, y, z)];
  }
}

__device__ vector trilinear_interpolation(vector v0, int n_x, int n_y, int n_z, vector_field field){
  int x1, y1, z1, x0, y0, z0;
  double xd, yd, zd;
  
  vector P1, P2, P3, P4, P5, P6, P7, P8, X1, X2, X3, X4, Y1, Y2, final;
  
  x1 = ceil(v0.x);
  y1 = ceil(v0.y);
  z1 = ceil(v0.z);
  x0 = floor(v0.x);
  y0 = floor(v0.y);
  z0 = floor(v0.z);
  xd = v0.x - x0;
  yd = v0.y - y0;
  zd = v0.z - z0;
  
  if(x1 >= n_x || y1 >= n_y || z1 >= n_z || x0 < 0 || y0 < 0 || z0 < 0){
    return nearest_neighbour(v0, n_x, n_y, n_z, field);
  }else{
    set(&P1, field[cuda_offset(n_x, n_y, x0, y0, z0)]);
    set(&P2, field[cuda_offset(n_x, n_y, x1, y0, z0)]);
    set(&P3, field[cuda_offset(n_x, n_y, x0, y0, z1)]);
    set(&P4, field[cuda_offset(n_x, n_y, x1, y0, z1)]);
    set(&P5, field[cuda_offset(n_x, n_y, x0, y1, z0)]);
    set(&P6, field[cuda_offset(n_x, n_y, x1, y1, z0)]);
    set(&P7, field[cuda_offset(n_x, n_y, x0, y1, z1)]);
    set(&P8, field[cuda_offset(n_x, n_y, x1, y1, z1)]);
    
    set(&X1, sum(P1, mult_scalar( subtract(P2, P1) , xd ) ));
    set(&X2, sum(P3, mult_scalar( subtract(P4, P3) , xd ) ));
    set(&X3, sum(P5, mult_scalar( subtract(P6, P5) , xd ) ));
    set(&X4, sum(P7, mult_scalar( subtract(P8, P7) , xd ) ));
    
    set(&Y1, sum(X1, mult_scalar( subtract(X3, X1) , yd ) ));
    set(&Y2, sum(X2, mult_scalar( subtract(X4, X2) , yd ) ));
    
    set(&final, sum(Y1, mult_scalar( subtract(Y2, Y1) , zd ) ));
    
    return final;
  }
}

/***********/
/* Kernels */
/***********/

__global__ void rk2_kernel(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector *points, int *n_points, int max_points){
  /*TODO: moving the field to the shared memory should increase performance*/
  vector k1, k2, initial, direction;
  int i, n_points_aux;
  
  n_points_aux = 0;
  
  i = threadIdx.x;
  
  set( &initial, v0[i] );
  set( &direction, field[cuda_offset(n_x, n_y, initial.x, initial.y, initial.z)] );
  
  while(module(direction) > 0.0 && (n_points_aux < max_points && n_points_aux < MAX_POINTS)){
    n_points_aux++;
        
    set( &(points[cuda_offset(count_v0, 0, i, n_points_aux - 1, 0)]), initial );
  
    set( &k1, mult_scalar( direction, h ) );
    set( &k2, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k1, 0.5 )), n_x, n_y, n_z, field), h) ); 
    
    set( &initial, sum( initial, k2) );
    set( &direction, trilinear_interpolation(initial, n_x, n_y, n_z, field) );
  }
  
  n_points[i] = n_points_aux;
  n_points_aux = 0;
}

__global__ void rk4_kernel(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector *points, int *n_points, int max_points){
  /*TODO: moving the field to the shared memory should increase performance*/
  vector k1, k2, k3, k4, initial, direction;
  int i, n_points_aux;
  
  n_points_aux = 0;
  
  i = threadIdx.x;
  
  set( &initial, v0[i] );
  set( &direction, field[cuda_offset(n_x, n_y, initial.x, initial.y, initial.z)] );
  
  while(module(direction) > 0.0 && (n_points_aux < max_points && n_points_aux < MAX_POINTS)){
    n_points_aux++;
        
    set( &(points[cuda_offset(count_v0, 0, i, n_points_aux - 1, 0)]), initial );
  
    set( &k1, mult_scalar( direction, h ) );
    set( &k2, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k1, 0.5 )), n_x, n_y, n_z, field), h) ); 
    set( &k3, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k2, 0.5 )), n_x, n_y, n_z, field), h) );
    set( &k4, mult_scalar( trilinear_interpolation(sum(initial, k3), n_x, n_y, n_z, field), h) );
    
    set( &initial, sum( initial, sum( mult_scalar( k1 , 0.166666667 ), sum( mult_scalar( k2, 0.333333333 ), sum( mult_scalar( k3, 0.333333333 ), mult_scalar( k4, 0.166666667 ) ) ) ) ) );
    set( &direction, trilinear_interpolation(initial, n_x, n_y, n_z, field) );
  }
  
  n_points[i] = n_points_aux;
}

/***********/
/* Callers */
/***********/

void rk2_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers){
  vector *d_v0;
  vector_field d_field;
  vector *d_points, *points_aux;
  int *d_n_points, *n_points_aux;
  int i, j, max_points;
  cudaEvent_t start, finish;
  float time;
  size_t available, total;
    
  cudaEventCreate(&start);
  cudaEventCreate(&finish);
  
  cudaEventRecord(start, 0);
  
  if(cudaMalloc(&d_v0, count_v0*sizeof(vector)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the initial points\n", (count_v0*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  if(cudaMalloc(&d_field, n_x*n_y*n_z*sizeof(vector)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the vector field\n", (n_x*n_y*n_z*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  if(cudaMalloc(&d_n_points, count_v0*sizeof(int)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the points count vector\n", (count_v0*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  cudaMemGetInfo(&available, &total);
  max_points = ((available*0.9)/(sizeof(vector)*count_v0));
  if(cudaMalloc(&d_points, count_v0*max_points*sizeof(vector)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the fibers\n", (count_v0*max_points*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  
  cudaMemcpy(d_v0, v0, count_v0*sizeof(vector), cudaMemcpyHostToDevice);
  cudaMemcpy(d_field, field, n_x*n_y*n_z*sizeof(vector), cudaMemcpyHostToDevice);

  /*TODO: adjust threads per block to maximize performance*/
  rk2_kernel<<<1,count_v0>>>(d_v0, count_v0, h, n_x, n_y, n_z, d_field, d_points, d_n_points, max_points);
  cudaDeviceSynchronize();

  n_points_aux = (int *) malloc(count_v0*sizeof(int));
  points_aux = (vector *) malloc(count_v0*max_points*sizeof(vector));;

  cudaMemcpy(n_points_aux, d_n_points, count_v0*sizeof(int), cudaMemcpyDeviceToHost);
  cudaMemcpy(points_aux, d_points, count_v0*max_points*sizeof(vector), cudaMemcpyDeviceToHost);

  cudaEventRecord(finish, 0);
  cudaEventSynchronize(finish);
  
  cudaEventElapsedTime(&time, start, finish);
  cudaEventDestroy(start);
  cudaEventDestroy(finish);
  
  printf("GPU time for RK2: %fs\n", time/1000.0);
  
  *fibers = (runge_kutta::Fiber *) malloc(count_v0*sizeof(runge_kutta::Fiber));
  for(i = 0; i < count_v0; i++){
    (*fibers)[i] = runge_kutta::Fiber(n_points_aux[i]);
    for(j = 0; j < n_points_aux[i]; j++){
      (*fibers)[i].setPoint(j, points_aux[runge_kutta::DataSet::offset(count_v0, 0, i, j, 0)]);
    }
  }
  
  cudaFree(d_v0);
  cudaFree(d_field);
  cudaFree(d_points);
  cudaFree(d_n_points);
}

void rk4_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, runge_kutta::Fiber **fibers){
  vector *d_v0;
  vector_field d_field;
  vector *d_points, *points_aux;
  int *d_n_points, *n_points_aux;
  int i, j, max_points;
  cudaEvent_t start, finish;
  float time;
  size_t available, total;

  cudaEventCreate(&start);
  cudaEventCreate(&finish);
  
  cudaEventRecord(start, 0);
  
  if(cudaMalloc(&d_v0, count_v0*sizeof(vector)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the initial points\n", (count_v0*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  if(cudaMalloc(&d_field, n_x*n_y*n_z*sizeof(vector)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the vector field\n", (n_x*n_y*n_z*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  if(cudaMalloc(&d_n_points, count_v0*sizeof(int)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the points count vector\n", (count_v0*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  cudaMemGetInfo(&available, &total);
  max_points = ((available*0.9)/(sizeof(vector)*count_v0));
  if(cudaMalloc(&d_points, count_v0*max_points*sizeof(vector)) == cudaErrorMemoryAllocation){
    printf("\nCould not allocate %fMB for the fibers\n", (count_v0*max_points*sizeof(vector))/1024.0/1024.0);
    exit(-1);
  }
  
  cudaMemcpy(d_v0, v0, count_v0*sizeof(vector), cudaMemcpyHostToDevice);
  cudaMemcpy(d_field, field, n_x*n_y*n_z*sizeof(vector), cudaMemcpyHostToDevice);
  
  
  /*TODO: adjust threads per block to maximize performance*/
  rk4_kernel<<<1,count_v0>>>(d_v0, count_v0, h, n_x, n_y, n_z, d_field, d_points, d_n_points, max_points);
  cudaDeviceSynchronize();
  
  n_points_aux = (int *) malloc(count_v0*sizeof(int));
  points_aux = (vector *) malloc(count_v0*max_points*sizeof(vector));
  
  cudaMemcpy(n_points_aux, d_n_points, count_v0*sizeof(int), cudaMemcpyDeviceToHost);
  cudaMemcpy(points_aux, d_points, count_v0*max_points*sizeof(vector), cudaMemcpyDeviceToHost);
  
  cudaEventRecord(finish, 0);
  cudaEventSynchronize(finish);
  
  cudaEventElapsedTime(&time, start, finish);
  cudaEventDestroy(start);
  cudaEventDestroy(finish);
  
  printf("GPU time for RK4: %fs\n", time/1000.0);
  
  *fibers = (runge_kutta::Fiber *) malloc(count_v0*sizeof(runge_kutta::Fiber));
  for(i = 0; i < count_v0; i++){
    (*fibers)[i] = runge_kutta::Fiber(n_points_aux[i]);
    for(j = 0; j < n_points_aux[i]; j++){
      (*fibers)[i].setPoint(j, points_aux[runge_kutta::DataSet::offset(count_v0, 0, i, j, 0)]);
    }
  }
  
  cudaFree(d_v0);
  cudaFree(d_field);
  cudaFree(d_points);
  cudaFree(d_n_points);
}