namespace runge_kutta{
  class Scene{
    private:
      ConeCollection _vector_field;
      CylinderCollection _rk2_cylinders;
      CylinderCollection _rk4_cylinders;
      GLdouble _x_angle;
      GLdouble _y_angle;
      GLdouble _translation_x;
      GLdouble _translation_y;
      GLdouble _translation_z;
      bool _display_rk2;
      bool _display_rk4;
      bool _display_vf;
      
      void renderVectorField();
      void renderAxis();
      void renderCylinders();
      void ilumination();
    public:
      Scene();
      Scene(DataSet data_set, unsigned fibers_count, Fiber *rk2_fibers, Fiber *rk4_fibers);
      void render();
      void increaseXAngle();
      void decreaseXAngle();
      void increaseYAngle();
      void decreaseYAngle();
      void increaseX();
      void increaseY();
      void decreaseX();
      void decreaseY();
      void increaseScale();
      void decreaseScale();
      void toogleRK2();
      void toogleRK4();
      void toogleDS();
  };
}
