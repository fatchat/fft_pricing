#include <cstdio>
#include <vector>
#include <complex>
#include <cstdlib>
#include <fftw3.h>
#include <cassert>

#include "csutils/globals.h"
#include "mathutils/fft.h"

typedef std::vector<CMP> cmpvec;

void ft_gsl(size_t N, const cmpvec& little_x, cmpvec& big_X);
void ft_bf(size_t N, const cmpvec& little_x, cmpvec& big_X);
void ft_fftw(size_t N, const cmpvec& little_x, cmpvec& big_X);
void ft_fftw_d(size_t rank, const int* N, const cmpvec&, cmpvec&);

void print_both(const cmpvec& a, const cmpvec& b);
void print_diff(const cmpvec& a, const cmpvec& b);

int main(int argc, char* argv[])
{
  const size_t size = (argc > 1) ? atoi(argv[1]) : 32;
  cmpvec inputData;
  for (size_t i = 0; i < size; ++i) {
    inputData.push_back(CMP(0.1 + i, 0.2 * i));
  }

  cmpvec bf;
  ft_bf(size, inputData, bf);

  if (0) {
    cmpvec gsl;
    ft_gsl(size, inputData, gsl);
    print_diff(bf, gsl);
  }

  if(0) {
    cmpvec fftw;
    ft_fftw(size, inputData, fftw);
    print_diff(bf, fftw);
  }

  if(0) {
    cmpvec fftw_d;
    const int isz = size;
    ft_fftw_d(1, &isz, inputData, fftw_d);
    print_diff(bf, fftw_d);
  }

  if(1) {
    cmpvec fftw_d1;
    const int sz1[] = { 3, 5 };
    ft_fftw_d(2, &sz1[0], inputData, fftw_d1);

    cmpvec fftw_d2;
    const int sz2[] = { 5, 3  };
    ft_fftw_d(2, &sz2[0], inputData, fftw_d2);

    print_both(fftw_d1, fftw_d2);
  }

  return 0;
}

// display routines
void print_both(const cmpvec& a, const cmpvec& b)
{
  assert(a.size() == b.size());
  const size_t size = a.size();
  for(size_t ii = 0; ii < size; ++ii) {
    printf("%.2f+%.2fi vs %.2f+%.2fi\n",
    	   a[ii].real(), a[ii].imag(),
	   b[ii].real(), b[ii].imag());
  }
}

void print_diff(const cmpvec& a, const cmpvec& b)
{
  assert(a.size() == b.size());
  const size_t size = a.size();
  for(size_t ii = 0; ii < size; ++ii) {
    const CMP diff = a[ii] - b[ii];
    printf("%.5f+%.5fi\n", diff.real(), diff.imag());
  }
}

// brute force method to test against
void ft_bf(size_t N, const cmpvec& little_x, cmpvec& big_X)
{
  big_X.clear();
  for (size_t m = 0; m < N; ++m) {
    CMP X_m(0,0);
    for (size_t j = 0; j < N; ++j) {
      CMP expFactor = exp(- c_I * (2.0 * PI / N * j * m));
      X_m += expFactor * little_x[j];
    }
    big_X.push_back(X_m);
  }
}

// ft using GSL
void ft_gsl(size_t N, const cmpvec& little_x, cmpvec& big_X)
{
  big_X.clear();
  FFTCalc fftCalc(N);
  fftCalc.setData(little_x);
  fftCalc.performFFT();
  fftCalc.getData(big_X);
  assert(little_x.size() == big_X.size());
}

// using fftw lib
void ft_fftw(size_t N, const cmpvec& little_x, cmpvec& big_X)
{
  big_X.resize(N);
  cmpvec nclx(little_x);
  // http://www.fftw.org/fftw3_doc/Complex-numbers.html
  fftw_complex *in = reinterpret_cast<fftw_complex*>(&nclx[0]);
  fftw_complex *out = reinterpret_cast<fftw_complex*>(&big_X[0]);
  fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
}

// multi-d fftw
void ft_fftw_d(size_t rank, const int* N,
	       const cmpvec& little_x, cmpvec& big_X)
{
  size_t final_size = 1;
  for (size_t ii = 0; ii < rank; ++ii) final_size *= N[ii];
  big_X.resize(final_size);
  // http://www.fftw.org/fftw3_doc/Complex-Multi_002dDimensional-DFTs.html
  cmpvec nclx(little_x);
  fftw_complex *in = reinterpret_cast<fftw_complex*>(&nclx[0]);
  fftw_complex *out = reinterpret_cast<fftw_complex*>(&big_X[0]);
  fftw_plan plan = fftw_plan_dft(rank, N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
}
