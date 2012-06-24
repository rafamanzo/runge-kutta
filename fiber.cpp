#include<cstdlib>
#include "dataset.h"
#include "fiber.h"

using namespace runge_kutta;

Fiber::Fiber(unsigned pointsCount){
  _pointsCount = pointsCount;
  
  _points = (vector *) malloc(pointsCount*sizeof(vector));
}

void Fiber::setPoint(unsigned order, vector point){
  _points[order] = point;
}
vector Fiber::getPoint(unsigned order){
  return _points[order];
}
unsigned Fiber::pointsCount(){
  return _pointsCount;
}
