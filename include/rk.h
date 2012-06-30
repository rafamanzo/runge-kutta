namespace runge_kutta{
  class RungeKutta{
    private:
      DataSet _dataset;
      vector *_v0;
      unsigned _count_v0;
      double _h;
    public:
      RungeKutta(DataSet dataset, vector *v0, unsigned count_v0, double h); 
      Fiber *order2();
      Fiber *order4();
  };
}
