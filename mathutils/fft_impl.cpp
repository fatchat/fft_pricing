#include <cassert>

#include "mathutils/fft.h"

FFTCalc::FFTCalc(unsigned int length)
  : N_(length)
  , data_(2 * N_)
  , wavetable_(0)
  , workspace_(0)
{
  assert(N_ > 0);
  wavetable_ = gsl_fft_complex_wavetable_alloc(N_);
  workspace_ = gsl_fft_complex_workspace_alloc(N_);
}

FFTCalc::~FFTCalc()
{
  gsl_fft_complex_wavetable_free(wavetable_);
  gsl_fft_complex_workspace_free(workspace_);
}


void
FFTCalc::getData(std::vector<std::complex<double> >& outputData) const
{
  outputData.resize(N_);
  for (unsigned int i = 0; i < N_; ++i) {
    const std::complex<double> a (data_[2 * i], data_[2 * i + 1]);
    outputData[i] = a;
  }
}

void
FFTCalc::setData(const std::vector<std::complex<double> > data)
{
  assert (data.size() == N_);
  for (unsigned int i = 0; i < N_; ++i) {
    data_[2 * i] = data[i].real();
    data_[2 * i + 1] = data[i].imag();
  }
}
  
int
FFTCalc::performFFT() 
{
  return
    gsl_fft_complex_forward(&data_[0], 1, N_, wavetable_, workspace_);
}
