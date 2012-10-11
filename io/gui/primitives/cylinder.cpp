/************************************************************************************************/
/* The original render method was written by Curran Kelleher in 2008 and was modified by us     */
/* with corrections mostly on the cylinder direction                                            */
/*                                                                                              */
/* The original method is under public domain and can be found in:                              */
/* http://lifeofaprogrammergeek.blogspot.com.br/2008/08/opengl-example-rendering-cylinders.html */
/************************************************************************************************/

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glut.h>
#endif

#include<cmath>
#include<cstdio>
#include<cylinder.h>

#define CYLINDER_RADIUS 0.25
#define CYLINDER_SUBDIVISIONS 8

using namespace runge_kutta;

Cylinder::Cylinder(double x1, double y1, double z1, double x2, double y2, double z2){
  _x1 = x1;
  _y1 = y1;
  _z1 = z1;
  _x2 = x2;
  _y2 = y2;
  _z2 = z2;
}

void Cylinder::render(GLfloat red, GLfloat green, GLfloat blue, GLdouble angle_x, GLdouble angle_y){
  GLdouble vx, vy, vz, v, ax;
  GLUquadricObj *_quadric;
  
  _quadric = gluNewQuadric();
  //gluQuadricDrawStyle(_quadric, GLU_LINE); /* wireframe */
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
  
  if( vz < 0.0 ) ax = -ax;
  
  glPushMatrix();
    glColor3f(red, green, blue);
    glRotated(angle_x, 1.0, 0.0, 0.0);
    glRotated(angle_y, 0.0, 1.0, 0.0);
  
    //draw the cylinder body
    glTranslated( _x1, _y1, _z1 );
    glRotated(ax, -vy, vx, 0.0);
    gluQuadricOrientation(_quadric,GLU_OUTSIDE);
    gluCylinder(_quadric, CYLINDER_RADIUS, CYLINDER_RADIUS, v, CYLINDER_SUBDIVISIONS, 1);

    //draw the first cap
    gluQuadricOrientation(_quadric,GLU_INSIDE);
    gluDisk(_quadric, 0.0, CYLINDER_RADIUS, CYLINDER_SUBDIVISIONS, 1);
    glTranslated( 0,0,v );

    //draw the second cap
    gluQuadricOrientation(_quadric,GLU_OUTSIDE);
    gluDisk(_quadric, 0.0, CYLINDER_RADIUS, CYLINDER_SUBDIVISIONS, 1);
  glPopMatrix();  
    
  gluDeleteQuadric(_quadric);
}
