#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../vector_field.h"
#include "plot.h"
#include "math_operations.h"
#include "../c/vector_operations.h"

int n_x, n_y, n_z, v0_count;
int* n_points;
vector** points;
int left_button = 0;
int right_button = 0;
double min, max, ratio;
double eye_x = 6.0;
double eye_y = -6.0;
double eye_z = -100.0;
int mouse_start_x, mouse_start_y;

static void resize(int width, int height){
  glMatrixMode(GL_MODELVIEW); 
  glViewport(0, 0, width, height);
  gluLookAt(0.0, 0.0, 0.0, eye_x, eye_y, eye_z, 0.0, 1.0, 0.0);
  glutPostRedisplay();
}

static void plot_vectors(){
  int i, k;
  double mod;
  /* variavel pendentes */
  double r = 5;

  glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(-1,-1.0,0.0);
    glRotated(180,0,1,0); 
    glRotated(180,0,0,1);  
  glPushMatrix();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(0,0,1);

  for(k = 0; k < v0_count; k++){
    for(i = 0; i < n_points[k]; i++){

        mod = module(points[k][i]);
        glPushMatrix();
         glTranslated((points[k][i].x/min)*ratio,-(points[k][i].y/min)*ratio,(points[k][i].z/min)*ratio);
         glRotated(angle_y(points[k][i]),0,1,0);
         glRotated(angle_x(points[k][i]),1,0,0); 
         glRotated(angle_z(points[k][i]),0,0,1);
         glutSolidSphere((r/max)*ratio/*,(mod/max)*ratio*/,SLICES,STACKS);
        glPopMatrix();
      }
    }
 glutSwapBuffers();
}

void mouse_click(int button, int state, int x, int y){
  if(state == GLUT_DOWN){
    if(button == GLUT_LEFT_BUTTON){
      left_button = 1;
    }else if(button == GLUT_RIGHT_BUTTON){
      right_button = 1;
    }
    mouse_start_x = x;
    mouse_start_y = y;
  }else{
    if(button == GLUT_LEFT_BUTTON){
      left_button = 0;
    }else if(button == GLUT_RIGHT_BUTTON){
      right_button = 0;
    }
  }
}

void mouse_move(int x, int y){
  if(left_button == 1){
    eye_x += (mouse_start_x - x)*ratio;
    eye_y += (mouse_start_y - y)*ratio;
  }else if(right_button == 1){
    eye_z += (mouse_start_y - y)*ratio;
  }
  
  mouse_start_x = x;
  mouse_start_y = y;
  
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0, eye_x, eye_y, eye_z, 0.0, 1.0, 0.0);
  glutPostRedisplay();;
}

void plot_main(int argc, char *argv[]){

  glutInit(&argc, argv);
  glutInitWindowSize(1024,768);
  glutInitWindowPosition(0,0);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Runge-Kutta");

  glClearColor(1,1,1,1);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

  glutReshapeFunc(resize);
  glutDisplayFunc(plot_vectors);
  glutMotionFunc(mouse_move);
  glutMouseFunc(mouse_click);
  glutMainLoop();
}

void plot_init(int argc, char *argv[], int nX, int nY, int nZ, int v0Count, int *n_pts, vector **pts){
  int i, j;
  double mod;

  n_x = nX;
  n_y = nY;
  n_z = nZ;
  v0_count = v0Count;
  min = nX*nY*nZ;
  max = -min;
 
  n_points = (int*) malloc(sizeof(int)*v0_count);
  for(i = 0; i < v0_count; i++)
    n_points[i] = n_pts[i];

  points  = (vector**) malloc(sizeof(vector)*v0_count);
  for(i = 0; i < v0_count; i++){
    points[i] = (vector*) malloc(sizeof(vector)*n_points[i]);
    for(j = 0; j < n_points[i]; j++){
      points[i][j].x = pts[i][j].x;
      points[i][j].y = pts[i][j].y;
      points[i][j].z = pts[i][j].z;
      mod = module(points[i][j]);
      if( mod > max)
        max = mod;
      else if( mod < min)
        min = mod;
    }
  }
  ratio = min/max;
  plot_main(argc,argv);
}
