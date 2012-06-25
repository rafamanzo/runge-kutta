#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdio.h>
#include "../dataset.h"
#include "../fiber.h"
#include "rk_kernel.h"

using namespace runge_kutta;

vector sum(vector v1, vector v2){
  vector sum;
  
  sum.x = v1.x + v2.x;
  sum.y = v1.y + v2.y;
  sum.z = v1.z + v2.z;
  
  return sum;
}

vector mult_scalar(vector v, double scalar){
  vector mult;
  
  mult.x = v.x*scalar;
  mult.y = v.y*scalar;
  mult.z = v.z*scalar;
  
  return mult;
}

void set(vector *x, vector y){
  (*x).x = y.x;
  (*x).y = y.y;
  (*x).z = y.z;
}

double module(vector v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

double distance(vector x, vector y){
  return module(sum(x, mult_scalar(y, -1.0)));
}

vector nearest_neighbour(vector v0, int n_x, int n_y, int n_z, vector_field field){
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

  if(x >= n_x || y >= n_y || z >= n_z || x < 0 || y < 0 || z < 0)
    return zero;
  else
    return field[DataSet::offset(n_x, n_y, x, y, z)];
}

vector trilinear_interpolation(vector v0, int n_x, int n_y, int n_z, vector_field field){
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
    
    set(&i1, sum( mult_scalar(field[DataSet::offset(n_x, n_y, x0, y0, z0)], (1.0 - zd)), mult_scalar(field[DataSet::offset(n_x, n_y, x0, y0, z1)], zd) ) );
    set(&i2, sum( mult_scalar(field[DataSet::offset(n_x, n_y, x0, y1, z0)], (1.0 - zd)), mult_scalar(field[DataSet::offset(n_x, n_y, x0, y1, z1)], zd) ) );
    set(&j1, sum( mult_scalar(field[DataSet::offset(n_x, n_y, x1, y0, z0)], (1.0 - zd)), mult_scalar(field[DataSet::offset(n_x, n_y, x1, y0, z1)], zd) ) );
    set(&j2, sum( mult_scalar(field[DataSet::offset(n_x, n_y, x1, y1, z0)], (1.0 - zd)), mult_scalar(field[DataSet::offset(n_x, n_y, x1, y1, z1)], zd) ) );
    
    set(&w1, sum( mult_scalar(i1, (1.0 - yd)), mult_scalar(i2, yd) ) );
    set(&w2, sum( mult_scalar(j1, (1.0 - yd)), mult_scalar(j2, yd) ) );
    
    return sum( mult_scalar(w1, (1.0 - xd)), mult_scalar(w2, xd) );
  }
}

void rk2_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, Fiber **fibers){
  vector k1, k2, initial, direction;
  vector *points_aux;
  int i, j, n_points_aux;
  clock_t start, finish;
  
  points_aux = NULL;
  n_points_aux = 0;
  
  start = clock();
  
  *fibers = (Fiber *) malloc(count_v0*sizeof(Fiber *));
  
  for(i = 0; i < count_v0; i++){
    set( &initial, v0[i] );
    set( &direction, field[DataSet::offset(n_x, n_y, initial.x, initial.y, initial.z)] );
    
    while(floor(module(direction)) > 0.0){
      n_points_aux++;
      points_aux = (vector *) realloc(points_aux, n_points_aux*sizeof(vector));
          
      set( &(points_aux[n_points_aux - 1]), initial);
    
      set( &k1, mult_scalar( direction, h ) );
      set( &k2, sum( mult_scalar(k1, 0.5), mult_scalar( direction, h ) ) );
      
      set( &initial, sum( initial, k2) );
      set( &direction, trilinear_interpolation(initial, n_x, n_y, n_z, field) );
    }
    
    (*fibers)[i] = Fiber(n_points_aux);
    for(j = 0; j < n_points_aux; j++)
      (*fibers)[i].setPoint(j, points_aux[j]);
    points_aux = NULL;
    n_points_aux = 0;
  }
  
  finish = clock();
  
  printf("CPU time for RK2: %fs\n", ((double) (finish - start))/CLOCKS_PER_SEC);
}

void rk4_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, Fiber **fibers){
  vector k1, k2, k3, k4, initial, direction;
  vector *points_aux;
  int i, j, n_points_aux;
  clock_t start, finish;
  
  points_aux = NULL;
  n_points_aux = 0;
  
  start = clock();
  
  *fibers = (Fiber *) malloc(count_v0*sizeof(Fiber *));
    
  for(i = 0; i < count_v0; i++){
    set( &initial, v0[i] );
    set( &direction, field[DataSet::offset(n_x, n_y, initial.x, initial.y, initial.z)] );
    
    while(floor(module(direction)) > 0.0){
      n_points_aux++;
      points_aux = (vector *) realloc(points_aux, n_points_aux*sizeof(vector));
          
      set( &(points_aux[n_points_aux - 1]), initial);
    
      set( &k1, mult_scalar( direction, h ) );
      set( &k2, sum( mult_scalar(k1, 0.5), mult_scalar( direction, h ) ) );
      set( &k3, sum( mult_scalar(k2, 0.5), mult_scalar( direction, h ) ) );
      set( &k4, sum( k3, mult_scalar( direction, h ) ) );
      
      set( &initial, sum( initial, sum( mult_scalar( k1 , 0.166666667 ), sum( mult_scalar( k2, 0.333333333 ), sum( mult_scalar( k3, 0.333333333 ), mult_scalar( k4, 0.166666667 ) ) ) ) ) );
      set( &direction, trilinear_interpolation(initial, n_x, n_y, n_z, field) );
    }
    
    (*fibers)[i] = Fiber(n_points_aux);
    for(j = 0; j < n_points_aux; j++)
      (*fibers)[i].setPoint(j, points_aux[j]);
    points_aux = NULL;
    n_points_aux = 0;
  }
  
  finish = clock();
  printf("CPU time for RK4: %fs\n", ((double) (finish - start))/CLOCKS_PER_SEC);
}
