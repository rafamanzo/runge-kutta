namespace runge_kutta{
  class Input{
    private:
      FILE *fp;      
    public:
      Input(char *file_name);
      DataSet parse(double *h, vector **v0, int *v0_count);
  };
}
