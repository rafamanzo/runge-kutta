/* f(x,y) = dydx = a*x + b*y + c */
typedef struct firsrorderfunction {
  double a;
  double b;
  double c;
} fof;

void rk2(double *x0, double *y0, int n, double h, fof *dydx, double *y1);
void rk4(double *x0, double *y0, int n, double h, fof *dydx, double *y1);
