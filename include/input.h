namespace runge_kutta{
  class Input{
    private:
      char *_file_name;
      unsigned _file_type;
      DataSet parseNative(double *h, vector **v0, unsigned *v0_count);
      DataSet parseAnalyze(double *h, vector **v0, unsigned *v0_count);
    public:
      static const unsigned NATIVE_TYPE = 0;
      static const unsigned ANALYZE_TYPE = 1;
      Input(char *file_name, unsigned file_type);
      Input();
      DataSet parse(double *h, vector **v0, unsigned *v0_count);
  };
}
