#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>
#include <complex>
#include <cassert>

#include "pricing/fteurocalc.h"
#include "mathutils/fft.h"

FTEuroCalc::FTEuroCalc(double r, double sigma, double time, double logS,
		       double alpha, unsigned int N, double dpsi)
  : r_(r), sigma_(sigma), time_(time), logS_(logS)
  , alpha_(alpha), N_(N), dpsi_(dpsi)
  , R_((r_ - 0.5 * pow(sigma_,2)) * time_)
  , lambda_(2 * PI / dpsi_ / N_)
{ 
  assert (N_ > 0);
  assert (dpsi_ > 0);
}

std::vector<double>
FTEuroCalc::createLogStrikes(double h_0) const
{
  std::vector<double> logStrikes(N_);
  logStrikes[0] = h_0;
  for (unsigned int j = 1; j < N_; ++j) {
    logStrikes[j] = logStrikes[j - 1] + lambda_;
  }
  return logStrikes;
}

CMP
FTEuroCalc::c_hat(double psi) const
{
  const CMP ft_euro = phi_T_hat(psi - (alpha_ + 1.0) * c_I);
  const CMP num = exp(-r_ * time_) * ft_euro;
  const CMP dem = 
    pow(alpha_,2) + alpha_ - pow(psi,2) + c_I * (2 * alpha_ + 1.0) * psi;
  
  return num / dem;
}

CMP
FTEuroCalc::phi_T_hat(CMP eta) const
{
  return exp(c_I * (logS_ + R_) * eta - 0.5 * pow(sigma_,2) * pow(eta,2) * time_);
}

void
FTEuroCalc::create_x_vector(double logK,
			    std::vector<CMP> &outputData) const
// this works because when used by callPrice it gives the same results as
// priceWithBS()
{
  outputData.clear();
  for (unsigned int j = 0; j < N_; ++j) {
    const double psi = dpsi_ * (j + 0.5);
    const CMP expFactor = exp(-c_I * logK * psi);
    const CMP chat = c_hat(psi);
    outputData.push_back(chat * expFactor);
  }
}

void
FTEuroCalc::create_X_vector(const std::vector<CMP> &little_x,
			    std::vector<CMP> &big_X) const
// this works because it does what the brute force loop does
{
  big_X.clear();
  //std::cout << "FFTCALC\n";
  FFTCalc fftCalc(N_);
  fftCalc.setData(little_x);
  fftCalc.performFFT();
  fftCalc.getData(big_X);
}

void
FTEuroCalc::create_X_vector_BF(const std::vector<CMP> &little_x,
			       std::vector<CMP> &big_X) const
// not optimized, no STL algorithms, just follow the def.
{
  //std::cout << "BRUTEFORCE\n";
  for (unsigned int m = 0; m < N_; ++m) {
    CMP X_m;
    for (unsigned int j = 0; j < N_; ++j) {
      CMP expFactor = exp(- c_I * (2.0 * PI / N_ * j * m));
      X_m += expFactor * little_x[j];
    }
    big_X.push_back(X_m);
  }
}

double FTEuroCalc::formula_6_CallPrice(double logK) const
{
  std::vector<CMP> little_x;
  create_x_vector(logK, little_x);

  std::vector<double> realParts(little_x.size());
  std::transform(little_x.begin(), little_x.end(),
		 realParts.begin(),
		 std::mem_fun_ref<double&, CMP>(&CMP::real));

  const double sum
    = std::accumulate(realParts.begin(), realParts.end(), 0.0);

  const double front = dpsi_ / (PI * exp(alpha_ * logK));
  const double price = front * sum;
  return price;
}

void
FTEuroCalc::fftCallPrices(double h_0, std::vector<double>& prices,
			  const double *logStrikes) const
{
  std::vector<CMP> little_x;
  create_x_vector(h_0, little_x);
  std::vector<CMP> big_X;
  create_X_vector(little_x, big_X);

  assert (little_x.size() == N_);
  assert (big_X.size() == N_);
  
  const double* useLogStrikes = 0;
  std::vector<double> computeLogStrikes;
  if (logStrikes) {
    useLogStrikes = logStrikes;
  }
  else {
    computeLogStrikes = createLogStrikes(h_0);
    useLogStrikes = &computeLogStrikes[0];
  }
  for (unsigned int m = 0; m < N_; ++m) {
    const CMP expFactor = exp(-c_I * PI * (m * 1.0/ N_));
    const double r_part = (expFactor * big_X[m]).real();
    const double h_m = useLogStrikes[m];
    const double outsideExpFactor = exp(-alpha_ * h_m);
    prices.push_back(dpsi_ / PI * outsideExpFactor * r_part);
  } 
}
