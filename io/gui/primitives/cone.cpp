/************************************/
/* Adaptation of the cylinder class */
/************************************/

#include<GL/glut.h>
#include<GL/gl.h>
#include<cmath>
#include<cstdio>
#include<cone.h>

#define CONE_RADIUS 0.1
#define CONE_SUBDIVISIONS 36

using namespace runge_kutta;

Cone::Cone(double x1, double y1, double z1, double x2, double y2, double z2){
  _x1 = x1;
  _y1 = y1;
  _z1 = z1;
  _x2 = x2;
  _y2 = y2;
  _z2 = z2;
}

void Cone::render(GLfloat red, GLfloat green, GLfloat blue, GLdouble angle_x, GLdouble angle_y){
  GLdouble vx, vy, vz, v, ax;
  GLUquadricObj *_quadric;
  
  _quadric = gluNewQuadric();
  gluQuadricNormals(_quadric, GLU_SMOOTH);
  
  vx = _x2 - _x1;
  vy = _y2 - _y1;
  vz = _z2 - _z1;

  //handle the degenerate case of z1 == z2 with an approximation
  if (vz > -0.000001 && vz < 0.000001){
    if (vz >= 0.0){ 
      vz = 0.000001;
    }else{
      vz = -0.000001;
    }
  }

  if (vy > -0.000001 && vy < 0.000001){
    if (vy >= 0.0){ 
      vy = -0.000001;
    }else{ 
      vy = 0.000001;
    }
  }

  if (vx > -0.000001 && vx < 0.000001){
    if (vx >= 0.0){ 
      vx = 0.000001;
    }else{ 
      vx = -0.000001;
    }
  }

  v = sqrt( vx*vx + vy*vy + vz*vz );
  ax = (180.0/M_PI)*acos( vz/v );
  
  /*if(v < 0)
    v = -CONE_HEIGHT;
  else
    v = CONE_HEIGHT;*/
    
  if( vz < 0.0 ) ax = -ax;
  
  glPushMatrix();
    glColor3f(red, green, blue);
    glRotated(angle_x, 1.0, 0.0, 0.0);
    glRotated(angle_y, 0.0, 1.0, 0.0);
  
    glTranslated( _x1, _y1, _z1 );
    glRotated(ax, -vy, vx, 0.0);
    gluQuadricOrientation(_quadric,GLU_OUTSIDE);
    gluCylinder(_quadric, CONE_RADIUS, 0.0f, v, CONE_SUBDIVISIONS, 1);
    
    //draw the cap
    //gluQuadricOrientation(_quadric,GLU_INSIDE);
    //gluDisk(_quadric, 0.0, CONE_RADIUS, CONE_SUBDIVISIONS, 1);
    //glTranslated( 0,0,v );
  glPopMatrix();  
    
  gluDeleteQuadric(_quadric);
}
