namespace runge_kutta{
  class Output{
    private:
      DataSet _dataSet;
      unsigned _fibersCount;
      Fiber *_rk2Fibers;
      Fiber *_rk4Fibers;
    public:
      Output(DataSet dataSet, unsigned fibersCount, Fiber *rk2Fibers, Fiber *rk4Fibers);
      void gnuplotInput();
      void gui();
  };
}
