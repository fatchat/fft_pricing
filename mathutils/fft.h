#include <complex>
#include <vector>
#include <gsl/gsl_fft_complex.h>

class FFTCalc {

  const unsigned int N_;
  std::vector<double> data_;
  gsl_fft_complex_wavetable* wavetable_;
  gsl_fft_complex_workspace* workspace_;

 public:
  FFTCalc(unsigned int length);
  ~FFTCalc();

  void getData(std::vector<std::complex<double> >& outputData) const;
  void setData(const std::vector<std::complex<double> > data);
  int performFFT();
};
