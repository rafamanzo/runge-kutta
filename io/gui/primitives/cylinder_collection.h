namespace runge_kutta{
  class CylinderCollection{
    private:
      unsigned _cylinder_count;
      Cylinder *_cylinders;
    public:
      CylinderCollection();
      void addCylinder(vector p1, vector p2);
      void render(GLfloat red, GLfloat green, GLfloat blue, GLdouble angle_x, GLdouble angle_y);
  };
}
