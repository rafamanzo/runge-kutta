typedef struct vec{
  double x;
  double y;
  double z;
} vector;

typedef vector *vector_field;

namespace runge_kutta{
  class DataSet{
    private:
      unsigned _n_x;
      unsigned _n_y;
      unsigned _n_z;
      vector_field _field;
    public:
      DataSet();
      DataSet(unsigned nx, unsigned ny, unsigned nz, vector_field field);
      unsigned n_x();
      unsigned n_y();
      unsigned n_z();
      vector_field field();
      vector field(unsigned x, unsigned y, unsigned z);
      static unsigned offset(unsigned nx, unsigned ny, unsigned x, unsigned y, unsigned z);
  };
}
