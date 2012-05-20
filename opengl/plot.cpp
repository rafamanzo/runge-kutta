#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../vector_field.h"
#include "plot.h"
#include "math_operations.h"
#include "../c/vector_operations.h"

int n_x, n_y, n_z, v0_count;
int *n_points_rk2, *n_points_rk4;
vector **points_rk2, **points_rk4;
int left_button = 0;
int right_button = 0;
double min_rk2, min_rk4, max_rk2, max_rk4, ratio_rk2, ratio_rk4;
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
  int i, j;
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

  /* RK-2 */
  glColor3d(0,0,1);
  for(i = 0; i < v0_count; i++){
    for(j = 0; j < n_points_rk2[i]; j++){
      mod = module(points_rk2[i][j]);
      glPushMatrix();
        glTranslated((points_rk2[i][j].x/min_rk2)*ratio_rk2,-(points_rk2[i][j].y/min_rk2)*ratio_rk2,(points_rk2[i][j].z/min_rk2)*ratio_rk2);
        glRotated(angle_y(points_rk2[i][j]),0,1,0);
        glRotated(angle_x(points_rk2[i][j]),1,0,0); 
        glRotated(angle_z(points_rk2[i][j]),0,0,1);
        glutSolidSphere((r/max_rk2)*ratio_rk2/*,(mod/max)*ratio*/,SLICES,STACKS);
      glPopMatrix();
    }
  }

  /* RK-4 */
  glColor3d(1,0,0);
  for(i = 0; i < v0_count; i++){
    for(j = 0; j < n_points_rk4[i]; j++){
      mod = module(points_rk4[i][j]);
      glPushMatrix();
        glTranslated((points_rk4[i][j].x/min_rk4)*ratio_rk4,-(points_rk4[i][j].y/min_rk4)*ratio_rk4,(points_rk4[i][j].z/min_rk4)*ratio_rk4);
        glRotated(angle_y(points_rk4[i][j]),0,1,0);
        glRotated(angle_x(points_rk4[i][j]),1,0,0); 
        glRotated(angle_z(points_rk4[i][j]),0,0,1);
        glutSolidSphere((r/max_rk4)*ratio_rk4/*,(mod/max)*ratio*/,SLICES,STACKS);
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
    eye_x += (mouse_start_x - x)/*ratio*/;
    eye_y += (mouse_start_y - y)/**ratio*/;
  }else if(right_button == 1){
    eye_z += (mouse_start_y - y)/*ratio*/;
  }
  
  mouse_start_x = x;
  mouse_start_y = y;
  
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0, eye_x, eye_y, eye_z, 0.0, 1.0, 0.0);
  glutPostRedisplay();
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

void plot_init(int argc, char *argv[], int nX, int nY, int nZ, int v0Count, int *n_pts_rk2, vector **pts_rk2, int *n_pts_rk4, vector **pts_rk4){
  int i, j;
  double mod;

  n_x = nX;
  n_y = nY;
  n_z = nZ;
  v0_count = v0Count;
  min_rk2 = min_rk4 =  nX*nY*nZ;
  max_rk2 = max_rk4 = -min_rk2;
 
  n_points_rk2 = (int*) malloc(sizeof(int)*v0_count);
  n_points_rk4 = (int*) malloc(sizeof(int)*v0_count);
  for(i = 0; i < v0_count; i++){
    n_points_rk2[i] = n_pts_rk2[i];
    n_points_rk4[i] = n_pts_rk4[i];
  }

  points_rk2  = (vector**) malloc(sizeof(vector)*v0_count);
  points_rk4  = (vector**) malloc(sizeof(vector)*v0_count);
  for(i = 0; i < v0_count; i++){
    points_rk2[i] = (vector*) malloc(sizeof(vector)*n_points_rk2[i]);
    points_rk4[i] = (vector*) malloc(sizeof(vector)*n_points_rk4[i]);

    for(j = 0; j < n_points_rk2[i]; j++){
      points_rk2[i][j].x = pts_rk2[i][j].x;
      points_rk2[i][j].y = pts_rk2[i][j].y;
      points_rk2[i][j].z = pts_rk2[i][j].z;
      mod = module(points_rk2[i][j]);
      if( mod > max_rk2)
        max_rk2 = mod;
      else if( mod < min_rk2)
        min_rk2 = mod;
    }

    for(j = 0; j < n_points_rk4[i]; j++){
      points_rk4[i][j].x = pts_rk4[i][j].x;
      points_rk4[i][j].y = pts_rk4[i][j].y;
      points_rk4[i][j].z = pts_rk4[i][j].z;
      mod = module(points_rk4[i][j]);
      if( mod > max_rk2)
        max_rk4 = mod;
      else if( mod < min_rk2)
        min_rk4 = mod;
    }


  }
  ratio_rk2 = min_rk2/max_rk2;
  ratio_rk4 = min_rk4/max_rk4;
  plot_main(argc,argv);
}
