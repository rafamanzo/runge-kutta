namespace runge_kutta{
  class Input{
    private:
      FILE *fp;      
    public:
      Input(char *file_name);
      void parse(int *n_x, int *n_y, int *n_z, double *h, vector **v0, int *v0_count, vector_field *field);
  };
}
