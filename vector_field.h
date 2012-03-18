typedef struct vec{
  double x;
  double y;
  double z;
} vector;

typedef vector *vector_field;

int offset(int n_x, int n_y, int x, int y, int z);
