namespace runge_kutta{
  class Cylinder{
    public:
      GLdouble _x1;
      GLdouble _y1;
      GLdouble _z1;
      GLdouble _x2;
      GLdouble _y2;
      GLdouble _z2;
      Cylinder(double x1, double y1, double z1, double x2, double y2, double z2);
      void render(GLfloat red, GLfloat green, GLfloat blue, GLdouble angle_x, GLdouble angle_y);
  };
}
