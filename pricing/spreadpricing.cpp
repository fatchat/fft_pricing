#include <cmath>

#include "pricing/spreadpricing.h"
#include "mathutils/gamma.h"
#include "csutils/globals.h"

SpreadPricing::SpreadPricing(double S1, double S2, double K,
			     double sigma1, double sigma2, double rho,
			     double r, double time)
  : S1_(S1), S2_(S2), K_(K), sigma1_(sigma1), sigma2_(sigma2), rho_(rho)
  , r_(r), time_(time), T1_(S1_/K_), T2_(S2_/K_), X1_(log(T1_)), X2_(log(T2_))
  , sigma1sq_(sigma1_ * sigma1_), sigma2sq_(sigma2_ * sigma2_)
  , sigma12rho_ (sigma1_ * sigma2_ * rho_)
  , v1_(c_I * time_ * (r_ - 0.5 * sigma1sq_))
  , v2_(c_I * time_ * (r_ - 0.5 * sigma2sq_))
{

}

CMP
SpreadPricing::spreadgamma_(CMP u1, CMP u2) const
{
  const CMP lognum =
    Gamma::log_gsl_gamma((c_I * (u1 + u2)) - CMP(1,0))
    + Gamma::log_gsl_gamma(-c_I * u2);
  const CMP logden = Gamma::log_gsl_gamma(c_I * u1 + CMP(1,0));

  const CMP a = u1 * v1_ + u2 * v2_;

  const CMP b = 0.5 * time_
    * (u1 * u1 * sigma1sq_ + u2 * u2 * sigma2sq_
       + 2 * sigma12rho_ * u1 * u2);

  const CMP phi = exp(a - b);					  
  const CMP psi = exp(c_I * (u1 * X1_ + u2 * X2_)) * phi;

  const CMP retVal = psi * exp(lognum -logden);
  return retVal;
}

CMP
SpreadPricing::spreadpricing(double e1,// we must have e1+e2<-1
			     double e2,//must take e2>0, e2=1 is fine
			     int N) const
{
  CMP sum(0,0);

  for(int i = -N; i < N; ++i) {
    for(int j = -N; j < N; ++j) {
      sum += spreadgamma_(0.25 * (i + 0.5) + c_I * e1,
			  0.25 * (j + 0.5) + c_I * e2);			 
    }
  }
  return exp(-r_ * time_) * K_ * sum / (4 * PI * PI);
}
