#include<cstdlib>
#include<cmath>
#include<dataset.h>

using namespace runge_kutta;

DataSet::DataSet(){
  _n_x = _n_y = _n_z = 0;
  _field = NULL;
}

DataSet::DataSet(unsigned nx, unsigned ny, unsigned nz, vector_field field){
  _n_x = nx;
  _n_y = ny;
  _n_z = nz;
  
  _field = field;
}

unsigned DataSet::n_x(){
  return _n_x;
}

unsigned DataSet::n_y(){
  return _n_y;
}

unsigned DataSet::n_z(){
  return _n_z;
}

vector_field DataSet::field(){
  return _field;
}

vector DataSet::field(unsigned x, unsigned y, unsigned z){
  return _field[offset(_n_x, _n_y, x, y, z)];
}

unsigned DataSet::offset(unsigned nx, unsigned ny, unsigned x, unsigned y, unsigned z){
  return x + nx*y + ny*nx*z;
}
