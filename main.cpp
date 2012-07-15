#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<dataset.h>
#include<fiber.h>
#include<rk.h>
#include<input.h>
#include<output.h>

using namespace runge_kutta;

int main(int argc, char *argv[]){
  unsigned v0_count;
  vector *v0;
  double h;
  Input file;
  if(strcmp(argv[1], "--analyze") != 0){
    file = Input(argv[1], Input::NATIVE_TYPE);
  }else{
    file = Input(argv[2], Input::ANALYZE_TYPE);
  }
  DataSet dataset = file.parse(&h, &v0, &v0_count);
  
  RungeKutta rk = RungeKutta(dataset, v0, v0_count, h);
  Fiber *rk4_fibers = rk.order4();
  Fiber *rk2_fibers = rk.order2();

  Output output = Output(dataset, v0_count, rk2_fibers, rk4_fibers);
  output.gnuplotInput();
  output.gui();
  
  return 0;
}
