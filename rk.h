namespace runge_kutta{
  class RungeKutta{
    private:
      DataSet _dataset;
      vector *_v0;
      unsigned _count_v0;
      double _h;
    public:
      RungeKutta(DataSet dataset, vector *v0, unsigned count_v0, double h); 
      void order2(vector ***points, int **n_points);
      void order4(vector ***points, int **n_points);
  };
}
