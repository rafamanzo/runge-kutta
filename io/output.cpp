#include<cstdlib>
#include<cstdio>
#include<GL/glut.h>
#include<dataset.h>
#include<fiber.h>
#include<cylinder.h>
#include<cylinder_collection.h>
#include<cone.h>
#include<cone_collection.h>
#include<scene.h>
#include<window_manager.h>
#include<output.h>

using namespace runge_kutta;

Output::Output(DataSet dataSet, unsigned fibersCount, Fiber *rk2Fibers, Fiber *rk4Fibers){
  _dataSet = dataSet;
  _fibersCount = fibersCount;
  _rk2Fibers = rk2Fibers;
  _rk4Fibers = rk4Fibers;
}

void Output::gnuplotInput(){
  FILE *rk2_dat, *rk4_dat, *rk2_vs_rk4;
  unsigned i,k;
  
  rk2_dat = fopen("rk2.dat", "w");
  rk4_dat = fopen("rk4.dat", "w");
  rk2_vs_rk4 = fopen("rk2-vs-rk4.p", "w");
  
  for(k = 0; k < _fibersCount; k++){
    for(i = 0; i < _rk2Fibers[k].pointsCount(); i++)
      fprintf(rk2_dat, "%f %f %f\n", _rk2Fibers[k].getPoint(i).x, _rk2Fibers[k].getPoint(i).y, _rk2Fibers[k].getPoint(i).z);
    fprintf(rk2_dat, "\n");
    
    for(i = 0; i < _rk4Fibers[k].pointsCount(); i++)
      fprintf(rk4_dat, "%f %f %f\n", _rk4Fibers[k].getPoint(i).x, _rk4Fibers[k].getPoint(i).y, _rk4Fibers[k].getPoint(i).z);
    fprintf(rk4_dat, "\n");
  }
  
  fclose(rk2_dat);
  fclose(rk4_dat);
  
  fprintf(rk2_vs_rk4, "set xrange[0:%d]\n", _dataSet.n_x() - 1);
  fprintf(rk2_vs_rk4, "set yrange[0:%d]\n", _dataSet.n_y() - 1);
  fprintf(rk2_vs_rk4, "set zrange[0:%d]\n", _dataSet.n_z() - 1);
  fprintf(rk2_vs_rk4, "splot \"rk2.dat\" w lines lt 1, \"rk4.dat\" w lines lt 2\n");
  fprintf(rk2_vs_rk4, "pause -1");

  fclose(rk2_vs_rk4);
}

void Output::gui(){
  WindowManager wm = WindowManager(Scene(_dataSet, _fibersCount, _rk2Fibers, _rk4Fibers));
  wm.loop();
}
