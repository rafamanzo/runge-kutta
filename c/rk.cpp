#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<stdio.h>
#include "../vector_field.h"
#include "../rk3d.h"
#include "vector_operations.h"

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

  if(x >= n_x || y >= n_y || z >= n_z || x < 0 || y < 0 || z < 0){
    return zero;
  }else{
    return field[offset(n_x, n_y, x, y, z)];
  }
}

void rk2(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  vector k1, k2, initial, direction;
  vector *points_aux;
  int i, n_points_aux;
  
  points_aux = NULL;
  n_points_aux = 0;
  
  for(i = 0; i < count_v0; i++){
    set( &initial, v0[i] );
    set( &direction, field[offset(n_x, n_y, initial.x, initial.y, initial.z)] );
    
    while(floor(module(direction)) > 0.0){
      n_points_aux++;
      points_aux = (vector *) realloc(points_aux, n_points_aux*sizeof(vector));
          
      set( &(points_aux[n_points_aux - 1]), initial);
    
      set( &k1, mult_scalar( direction, h ) );
      set( &k2, sum( mult_scalar(k1, 0.5), mult_scalar( direction, h ) ) );
      
      set( &initial, sum( initial, k2) );
      set( &direction, nearest_neighbour(initial, n_x, n_y, n_z, field) );
    }
    
    (*n_points)[i] = n_points_aux;
    (*points)[i] = points_aux;
    points_aux = NULL;
    n_points_aux = 0;
  }
}

void rk4(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, vector ***points, int **n_points){
  vector k1, k2, k3, k4, initial, direction;
  vector *points_aux;
  int i, n_points_aux;
  
  points_aux = NULL;
  n_points_aux = 0;
  
  for(i = 0; i < count_v0; i++){
    set( &initial, v0[i] );
    set( &direction, field[offset(n_x, n_y, initial.x, initial.y, initial.z)] );
    
    while(floor(module(direction)) > 0.0){
      n_points_aux++;
      points_aux = (vector *) realloc(points_aux, n_points_aux*sizeof(vector));
          
      set( &(points_aux[n_points_aux - 1]), initial);
    
      set( &k1, mult_scalar( direction, h ) );
      set( &k2, sum( mult_scalar(k1, 0.5), mult_scalar( direction, h ) ) );
      set( &k3, sum( mult_scalar(k2, 0.5), mult_scalar( direction, h ) ) );
      set( &k4, sum( k3, mult_scalar( direction, h ) ) );
      
      set( &initial, sum( initial, sum( mult_scalar( k1 , 0.166666667 ), sum( mult_scalar( k2, 0.333333333 ), sum( mult_scalar( k3, 0.333333333 ), mult_scalar( k4, 0.166666667 ) ) ) ) ) );
      set( &direction, nearest_neighbour(initial, n_x, n_y, n_z, field) );
    }
    
    (*n_points)[i] = n_points_aux;
    (*points)[i] = points_aux;
    points_aux = NULL;
    n_points_aux = 0;
  }
}
