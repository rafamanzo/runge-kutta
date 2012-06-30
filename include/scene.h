namespace runge_kutta{
  class Scene{
    private:
      CylinderCollection _rk2_cylinders;
      CylinderCollection _rk4_cylinders;
      GLdouble _scale;
      GLdouble _x_angle;
      GLdouble _y_angle;
      GLdouble _translation_x;
      GLdouble _translation_y;
      bool _display_rk2;
      bool _display_rk4;
      
      void renderAxis();
      void renderCylinders();
      void ilumination();
    public:
      Scene();
      Scene(unsigned fibers_count, Fiber *rk2_fibers, Fiber *rk4_fibers);
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
  };
}
