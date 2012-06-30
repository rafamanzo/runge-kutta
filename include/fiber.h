namespace runge_kutta{
  class Fiber{
    private:
      unsigned _pointsCount;
      vector *_points;
    public:
      Fiber(unsigned pointsCount);
      Fiber();
      void setPoint(unsigned order, vector point);
      vector getPoint(unsigned order);
      unsigned pointsCount();
  };
}
