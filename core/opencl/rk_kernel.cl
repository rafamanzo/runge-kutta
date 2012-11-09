#define MAX_POINTS 10000
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

typedef struct vec{
  double x;
  double y;
  double z;
} vector;

typedef vector* vector_field;

unsigned int opencl_offset(int n_x, int n_y, unsigned int x, unsigned int y, unsigned int z){
  return x + n_x*y + n_y*n_x*z;
}

vector mult_scalar(vector v, double scalar){
  vector mult;
  
  mult.x = v.x*scalar;
  mult.y = v.y*scalar;
  mult.z = v.z*scalar;
  
  return mult;
}

vector sum(vector v1, vector v2){
  vector sum;
  
  sum.x = v1.x + v2.x;
  sum.y = v1.y + v2.y;
  sum.z = v1.z + v2.z;
  
  return sum;
}

vector subtract(vector v1, vector v2){
  vector subtraction;
  
  subtraction.x = v1.x - v2.x;
  subtraction.y = v1.y - v2.y;
  subtraction.z = v1.z - v2.z;
  
  return subtraction;
}

double module(vector v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

void set(vector *v1, vector v2){
  (*v1).x = v2.x;
  (*v1).y = v2.y;
  (*v1).z = v2.z;
}

/************************************/
/* Auxiliary Aproximation Functions */
/************************************/

vector nearest_neighbour(vector v0, __global int* n_x,__global int* n_y,__global int* n_z, __global vector* field){
  int x, y, z;
  vector zero;
  
  zero.x = zero.y = zero.z = 0.0;
  
  if( (v0.x - floor(v0.x)) > 0.5 && v0.x < (*n_x - 1))
    x = (int) ceil(v0.x);
  else
    x = (int) floor(v0.x);
    
  if( (v0.y - floor(v0.y)) > 0.5 && v0.y < (*n_y - 1))
    y = (int) ceil(v0.y);
  else
    y = (int) floor(v0.y);
    
  if( (v0.z - floor(v0.z)) > 0.5 && v0.z < (*n_z - 1))
    z = (int) ceil(v0.z);
  else
    z = (int) floor(v0.z);

  if(x >= *n_x || y >= *n_y || z >= *n_z || x < 0 || y < 0 || z < 0){
    return zero;
  }else{
    return field[opencl_offset(*n_x, *n_y, x, y, z)];
  }
}
vector trilinear_interpolation(vector v0, __global int* n_x,__global int* n_y,__global int* n_z, __global vector* field){
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
  
  if(x1 >= *n_x || y1 >= *n_y || z1 >= *n_z || x0 < 0 || y0 < 0 || z0 < 0){
    return nearest_neighbour(v0, n_x, n_y, n_z, field);
  }else{
    set(&P1, field[opencl_offset(*n_x, *n_y, x0, y0, z0)]);
    set(&P2, field[opencl_offset(*n_x, *n_y, x1, y0, z0)]);
    set(&P3, field[opencl_offset(*n_x, *n_y, x0, y0, z1)]);
    set(&P4, field[opencl_offset(*n_x, *n_y, x1, y0, z1)]);
    set(&P5, field[opencl_offset(*n_x, *n_y, x0, y1, z0)]);
    set(&P6, field[opencl_offset(*n_x, *n_y, x1, y1, z0)]);
    set(&P7, field[opencl_offset(*n_x, *n_y, x0, y1, z1)]);
    set(&P8, field[opencl_offset(*n_x, *n_y, x1, y1, z1)]);
    
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

__kernel void rk2_kernel(__global vector *v0, __global unsigned int* count_v0,__global double* h,__global int* n_x,__global int* n_y,__global int* n_z, __global vector* field,__global vector *points,__global unsigned int *n_points, __global unsigned int* max_points){
  vector k1, k2, initial, direction;
  int i, n_points_aux;
  
  n_points_aux = 0;
  
  i = get_global_id(0);
  
  set( &initial, v0[i] );
  set( &direction, field[opencl_offset(*n_x, *n_y, (unsigned int) initial.x, (unsigned int) initial.y, (unsigned int) initial.z)] );
  
  while(module(direction) > 0.0 && (n_points_aux < (*max_points) && n_points_aux < MAX_POINTS)){
    n_points_aux++;
        
    set((vector*) &(points[opencl_offset((*count_v0), 0, i, n_points_aux - 1, 0)]), initial );
  
    set( &k1, mult_scalar( direction, *h ) );
    set( &k2, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k1, 0.5 )), n_x, n_y, n_z, field), *h) ); 
    
    set( &initial, sum( initial, k2) );
    set( &direction, trilinear_interpolation(initial, n_x, n_y, n_z, field) );
  }
  
  n_points[i] = n_points_aux;
  n_points_aux = 0;
}

__kernel void rk4_kernel(__global vector *v0, __global unsigned int* count_v0,__global double* h,__global int* n_x,__global int* n_y,__global int* n_z, __global vector* field,__global vector *points,__global unsigned int *n_points, __global unsigned int* max_points){
  vector k1, k2, k3, k4, initial, direction;
  unsigned int i, n_points_aux;
  
  n_points_aux = 0;

  i = get_global_id(0);

  set( &initial, v0[i] );
  set( &direction, field[opencl_offset(*n_x, *n_y,(unsigned int) initial.x,(unsigned int) initial.y,(unsigned int) initial.z)] );
  
  while(module(direction) > 0.0 && (n_points_aux < (*max_points) && n_points_aux < MAX_POINTS)){
    n_points_aux++;

    set( (vector*) &points[opencl_offset((*count_v0), 0, i, n_points_aux - 1, 0)], initial ); 
         
    set( &k1, mult_scalar( direction, *h ) );
    set( &k2, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k1, 0.5 )), n_x, n_y, n_z, field), *h) ); 
    set( &k3, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k2, 0.5 )), n_x, n_y, n_z, field), *h) );
    set( &k4, mult_scalar( trilinear_interpolation(sum(initial, k3), n_x, n_y, n_z, field), *h) );
    
    set( &initial, sum( initial, sum( mult_scalar( k1 , 0.166666667 ), sum( mult_scalar( k2, 0.333333333 ), sum( mult_scalar( k3, 0.333333333 ), mult_scalar( k4, 0.166666667 ) ) ) ) ) );
    set( &direction, trilinear_interpolation(initial, n_x, n_y, n_z, field) );
  }
  
  n_points[i] = n_points_aux;
}
