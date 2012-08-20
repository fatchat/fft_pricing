void gauss_legendre_tbl(int n, double* x, double* w, double eps);

template <class T>
double GaussLegendre<T>::integrate(int n, double a, double b)
{
  double* x = 0;
  double* w = 0;

  const int m = (n + 1) >> 1;
  
  /* Load appropriate predefined table */
  for (unsigned int i = 0; i < GLAW::GLAWSIZE; ++i) {
    if(n == GLAW::glaw[i].n) {
      x = GLAW::glaw[i].x;
      w = GLAW::glaw[i].w;
      break;
    }
  }
	
  /* Generate new if non-predefined table is required */
  /* with precision of 1e-10 */
  bool dtbl = false;
  if(!x) {
    dtbl = true;
    x = new double[m];
    w = new double [m];
    gauss_legendre_tbl(n,x,w,1e-10);
  }

  const double A = 0.5 * (b - a);
  const double B = 0.5 * (b + a);

  double s = 0.0;
  if(n & 1) {
    s = w[0]* (obj_.*fptr_)(B);
  }
  else {
    const double Ax = A*x[0];
    s = w[0] * ((obj_.*fptr_)(B+Ax) + (obj_.*fptr_)(B-Ax));
  }
  for (int i = 1; i < m; ++i) {
    const double Ax = A * x[i];
    s += w[i] * ((obj_.*fptr_)(B+Ax) + (obj_.*fptr_)(B-Ax));
  }

  if (dtbl) {
    delete[] x;
    delete[] w;
  }

  return A * s;
}
