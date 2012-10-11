#include<cstdlib>
#include<ctime>
#include<cmath>
#include<cstdio>
#include<pthread.h>
#include<dataset.h>
#include<fiber.h>
#include<rk_c_kernel.h>

using namespace runge_kutta;

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
    set(&P1, field[DataSet::offset(n_x, n_y, x0, y0, z0)]);
    set(&P2, field[DataSet::offset(n_x, n_y, x1, y0, z0)]);
    set(&P3, field[DataSet::offset(n_x, n_y, x0, y0, z1)]);
    set(&P4, field[DataSet::offset(n_x, n_y, x1, y0, z1)]);
    set(&P5, field[DataSet::offset(n_x, n_y, x0, y1, z0)]);
    set(&P6, field[DataSet::offset(n_x, n_y, x1, y1, z0)]);
    set(&P7, field[DataSet::offset(n_x, n_y, x0, y1, z1)]);
    set(&P8, field[DataSet::offset(n_x, n_y, x1, y1, z1)]);

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

void *rk2_kernel(void *args){
  int j, n_points_aux;
  vector k1, k2, initial, direction;
  vector *points_aux;
  kernel_args arguments = *( (kernel_args *) args );

  points_aux = NULL;
  n_points_aux = 0;

  set( &initial, arguments.v0[arguments.id] );
  set( &direction, arguments.field[DataSet::offset(arguments.n_x, arguments.n_y, initial.x, initial.y, initial.z)] );

  while(module(direction) > 0 && n_points_aux < MAX_POINTS){
    n_points_aux++;
    points_aux = (vector *) realloc(points_aux, n_points_aux*sizeof(vector));

    set( &(points_aux[n_points_aux - 1]), initial);

    set( &k1, mult_scalar( direction, arguments.h ) );
    set( &k2, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k1, 0.5 )), arguments.n_x, arguments.n_y, arguments.n_z, arguments.field), arguments.h) );

    set( &initial, sum( initial, k2) );
    set( &direction, trilinear_interpolation(initial, arguments.n_x, arguments.n_y, arguments.n_z, arguments.field) );
  }

  (*(arguments.fibers))[arguments.id] = Fiber(n_points_aux);
  for(j = 0; j < n_points_aux; j++)
    (*(arguments.fibers))[arguments.id].setPoint(j, points_aux[j]);

  return NULL;
}

void rk2_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, Fiber **fibers){
  int i;
  clock_t start, finish;
  kernel_args *arguments;
  pthread_t *threads, *threads_backup;

  start = clock();

  *fibers = (Fiber *) malloc(count_v0*sizeof(Fiber *));

  threads = (pthread_t *) malloc(count_v0*sizeof(pthread_t));
  threads_backup = (pthread_t *) malloc(count_v0*sizeof(pthread_t));
  arguments = (kernel_args *) malloc(count_v0*sizeof(kernel_args));
  for(i = 0; i < count_v0; i++){
    arguments[i].id = i;
    arguments[i].v0 = v0;
    arguments[i].count_v0 = count_v0;
    arguments[i].h = h;
    arguments[i].n_x = n_x;
    arguments[i].n_y = n_y;
    arguments[i].n_z = n_z;
    arguments[i].field = field;
    arguments[i].fibers = fibers;

    pthread_create(&(threads[i]), NULL, rk2_kernel, (void *) ( &(arguments[i]) ) );

    threads_backup[i] = threads[i];
  }

  for(i = 0; i < count_v0; i++)
    pthread_join(threads_backup[i], NULL);

  finish = clock();

  printf("CPU time for RK2: %fs\n", ((double) (finish - start))/((double) CLOCKS_PER_SEC));
}

void *rk4_kernel(void *args){
  int j, n_points_aux;
  vector k1, k2, k3, k4, initial, direction;
  vector *points_aux;
  kernel_args arguments = *( (kernel_args *) args );

  points_aux = NULL;
  n_points_aux = 0;

  set( &initial, arguments.v0[arguments.id] );
  set( &direction, arguments.field[DataSet::offset(arguments.n_x, arguments.n_y, initial.x, initial.y, initial.z)] );

  while(module(direction) > 0 && n_points_aux < MAX_POINTS){
    n_points_aux++;
    points_aux = (vector *) realloc(points_aux, n_points_aux*sizeof(vector));

    set( &(points_aux[n_points_aux - 1]), initial);

    set( &k1, mult_scalar( direction, arguments.h ) );
    set( &k2, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k1, 0.5 )), arguments.n_x, arguments.n_y, arguments.n_z, arguments.field), arguments.h) );
    set( &k3, mult_scalar( trilinear_interpolation(sum(initial, mult_scalar( k2, 0.5 )), arguments.n_x, arguments.n_y, arguments.n_z, arguments.field), arguments.h) );
    set( &k4, mult_scalar( trilinear_interpolation(sum(initial, k3), arguments.n_x, arguments.n_y, arguments.n_z, arguments.field), arguments.h) );

    set( &initial, sum( initial, sum( mult_scalar( k1 , 0.166666667 ), sum( mult_scalar( k2, 0.333333333 ), sum( mult_scalar( k3, 0.333333333 ), mult_scalar( k4, 0.166666667 ) ) ) ) ) );
    set( &direction, trilinear_interpolation(initial, arguments.n_x, arguments.n_y, arguments.n_z, arguments.field) );
  }

  (*(arguments.fibers))[arguments.id] = Fiber(n_points_aux);
  for(j = 0; j < n_points_aux; j++)
    (*(arguments.fibers))[arguments.id].setPoint(j, points_aux[j]);

  return NULL;
}

void rk4_caller(vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, Fiber **fibers){
  int i;
  clock_t start, finish;
  kernel_args *arguments;
  pthread_t *threads, *threads_backup;

  start = clock();

  *fibers = (Fiber *) malloc(count_v0*sizeof(Fiber *));

  threads = (pthread_t *) malloc(count_v0*sizeof(pthread_t));
  threads_backup = (pthread_t *) malloc(count_v0*sizeof(pthread_t));
  arguments = (kernel_args *) malloc(count_v0*sizeof(kernel_args));
  for(i = 0; i < count_v0; i++){
    arguments[i].id = i;
    arguments[i].v0 = v0;
    arguments[i].count_v0 = count_v0;
    arguments[i].h = h;
    arguments[i].n_x = n_x;
    arguments[i].n_y = n_y;
    arguments[i].n_z = n_z;
    arguments[i].field = field;
    arguments[i].fibers = fibers;

    pthread_create(&(threads[i]), NULL, rk4_kernel, (void *) ( &(arguments[i]) ) );

    threads_backup[i] = threads[i];
  }

  for(i = 0; i < count_v0; i++)
    pthread_join(threads_backup[i], NULL);

  finish = clock();

  printf("CPU time for RK4: %fs\n", ((double) (finish - start))/((double) CLOCKS_PER_SEC));
}