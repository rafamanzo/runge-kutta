vector sum(vector v1, vector v2);
vector mult_scalar(vector v, double scalar);
void set(vector *x, vector y);
double module(vector v);
double distance(vector x, vector y);
vector nearest_neighbour(vector v0, int n_x, int n_y, int n_z, vector_field field);
vector trilinear_interpolation(vector v0, int n_x, int n_y, int n_z, vector_field field);
