#include <complex>
#include <vector>

#include "csutils/globals.h"

class FTEuroCalc {

  const double r_; // interest rate
  const double sigma_; // volatility
  const double time_;
  const double logS_; // log strike
  const double alpha_;
  const unsigned int N_;
  const double dpsi_;

  const double R_;
  const double lambda_; // spacing between log strikes

 private:
  CMP c_hat(double psi) const;
  CMP phi_T_hat(CMP eta) const;

  void create_x_vector(double startingStrike,
		       std::vector<CMP> &outputData) const;

  void create_X_vector(const std::vector<CMP> &little_x,
		       std::vector<CMP>& big_X) const;

  void create_X_vector_BF(const std::vector<CMP> &little_x,
			  std::vector<CMP>& big_X) const;
  
 public:
  FTEuroCalc(double r, double sigma, double time, double logS,
	     double alpha, unsigned int N, double dpsi);

  std::vector<double> createLogStrikes(double h_0) const;

  double formula_6_CallPrice(double logK) const;

  void fftCallPrices(double h_0, std::vector<double>& prices,
		     const double* logStrikes = 0) const;
};
