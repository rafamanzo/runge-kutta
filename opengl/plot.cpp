#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../vector_field.h"
#include "plot.h"
#include "math_operations.h"
#include "../c/vector_operations.h"

vector_field field;
int start;
int vec;
double ratio;
double max_legth;
double cone_res;
double sphere_res;
int n_x, n_y, n_z;

int left_button = 0;
int right_button = 0;
double eye_x = 6.0;
double eye_y = -6.0;
double eye_z = -10.0;
int mouse_start_x, mouse_start_y;

static void resize(int width, int height){
    glViewport(0, 0, width, height);
    gluLookAt(0.0, 0.0, 0.0, eye_x, eye_y, eye_z, 0.0, 1.0, 0.0);
}

static void plot_vectors(){
  int i, j, k;
  double mod;
  
  /* variaveis pendentes */
  int d_x, d_y, d_z ;
  double max = 0.02;
  double ratio = 0.2;
  double r = 0.03;
  d_x = d_y = d_z = 1;
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(0,0,1);
 
  for(k = 0; k < n_z; k++){
    for(j = 0; j < n_y; j++){
      for(i = 0; i < n_x; i++){
        mod = module(field[offset(n_x,n_y,i,j,k)]);
        if( mod > max_legth )
          mod = max_legth;
        glPushMatrix();
         glTranslated((-n_x/2+i*d_x)*ratio,(-n_y/2+j*d_y)*ratio,(-n_z/2+k*d_z)*ratio);
          glRotated(angle_y(field[offset(n_x,n_y,i,j,k)]),0,1,0);
          glRotated(-angle_x(field[offset(n_x,n_y,i,j,k)]),1,0,0); 
          glRotated(angle_z(field[offset(n_x,n_y,i,j,k)]),0,0,1);
         glutSolidCone(pow(r/max,2)*ratio,(mod/max)*ratio,SLICES,STACKS);
        glPopMatrix();
       }
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
  plot_vectors();
}

void draw_main(int argc, char *argv[], int n_x, int n_y, int n_z, int v0_count, int *n_points, vector **points){

  //max_legth = sqrt(pow(field.d_x, 2) + pow(field.d_y, 2) + pow(field.d_z, 2));
  //ratio = (field.min/field.max)/max_legth;
  
  glutInit(&argc, argv);
  glutInitWindowSize(1024,768);
  glutInitWindowPosition(0,0);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Simulador");

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
