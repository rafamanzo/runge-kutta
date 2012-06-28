#include<cstdio>
#include<cstdlib>
#include<cstring>
#include "core/dataset.h"
#include "core/fiber.h"
#include "core/rk.h"
#include "io/input.h"
#include "io/output.h"

using namespace runge_kutta;

int main(int argc, char *argv[]){
  unsigned v0_count;
  vector *v0;
  double h;
  
  Input file = Input(argv[1]);
  DataSet dataset = file.parse(&h, &v0, &v0_count);
  
  RungeKutta rk = RungeKutta(dataset, v0, v0_count, h);
  Fiber *rk4_fibers = rk.order4();
  Fiber *rk2_fibers = rk.order2();

  Output output = Output(dataset, v0_count, rk2_fibers, rk4_fibers);
  output.gnuplotInput();
  output.gui();
  
  return 0;
}
