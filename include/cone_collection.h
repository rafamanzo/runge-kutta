namespace runge_kutta{
  class ConeCollection{
    private:
      unsigned _cone_count;
      Cone *_cones;
    public:
      ConeCollection();
      void addCone(vector p1, vector p2);
      void render(GLfloat red, GLfloat green, GLfloat blue, GLdouble angle_x, GLdouble angle_y);
  };
}
