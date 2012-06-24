namespace runge_kutta{
  class Fiber{
    private:
      unsigned _pointsCount;
      vector *_points;
    public:
      Fiber(unsigned pointsCount);
      void setPoint(unsigned order, vector point);
      vector getPoint(unsigned order);
      unsigned pointsCount();
  };
}
