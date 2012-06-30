#include<cstdlib>
#include<dataset.h>
#include<fiber.h>
#include<rk.h>
#include<rk_cuda_kernel.h>

using namespace runge_kutta;

RungeKutta::RungeKutta(DataSet dataset, vector *v0, unsigned count_v0, double h){
  _dataset = dataset;
  _v0 = v0;
  _count_v0 = count_v0;
  _h = h;
}

Fiber *RungeKutta::order2(){
  Fiber *fibers;
  
  rk2_caller(_v0, _count_v0, _h, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field(), &fibers);
  
  return fibers;
}

Fiber *RungeKutta::order4(){
  Fiber *fibers;
     
  rk4_caller(_v0, _count_v0, _h, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field(), &fibers);
  
  return fibers;
}
