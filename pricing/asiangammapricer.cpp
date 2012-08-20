#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>

#include "csutils/utils.h"
#include "mathutils/gamma.h"
#include "pricing/asiangammapricer.h"

CMP AsianGammaPricer::asiangamma(CMP csi, CMP lambda, double r, double sigma)
{
  const double nu = 2 * r / (sigma * sigma) - 1;
  CMP_nan_check_(1, nu, "nu");

  const CMP mu = sqrt(2.0 * lambda + nu * nu);
  CMP_nan_check_(1, mu, "mu");
 
  const CMP log_numerator = log(4.0)
    + Gamma::log_gsl_gamma(c_I * csi)
    + Gamma::log_gsl_gamma((mu + nu) / 2.0 + 1.0)
    + Gamma::log_gsl_gamma((mu - nu) / 2.0 - 1.0 - c_I * csi);
  CMP_nan_check_(1, log_numerator, "log_numerator");

  const CMP log_denominator = 2 * log(sigma)
    + log (lambda)
    + (1.0 + c_I * csi) * log (2)
    + Gamma::log_gsl_gamma((mu + nu) / 2.0 + 2.0 + c_I * csi)
    + Gamma::log_gsl_gamma((mu - nu) / 2.0);
  CMP_nan_check_(1, log_denominator, "log_denominator");

  const CMP log_retval = log_numerator - log_denominator;
  CMP_nan_check_(1, log_retval, "asiangamma:log_retval");

  const CMP retVal = exp(log_retval);
  CMP_nan_check_(1, retVal, "asiangamma:retVal");

  return retVal;
}

AsianGammaPricer::AsianGammaPricer(boost::shared_ptr<AgpData> data)
  : agp_data_(data)
  , asianPutPrice_(-1.0), asianCallPrice_(-1.0)
  , asianFloatCallPrice_(-1.0), asianFloatPutPrice_(-1.0)
{
  assert(agp_data_.get());
}

void AsianGammaPricer::newParams(boost::shared_ptr<AgpData> newParams, bool reset)
{
  assert(newParams.get());
  agp_data_ = newParams;
  if (reset) {
    asianPutPrice_ = -1.0; asianCallPrice_ = -1.0;
    asianFloatCallPrice_ = -1.0; asianFloatPutPrice_ = -1.0;
  }
}

unsigned int
AsianGammaPricer::create_inner_loop_summands_(std::vector<CMP>& xxRet,
					      unsigned int n) const
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  std::vector<CMP> xx;
  xx.resize(2 * agp.N_);
  const CMP xx_n = asiangamma(agp.delta_ * (n + agp.offset_ - agp.N_) + c_I * agp.af_,
			      agp.al_ + 0,
			      agp.r_,
			      agp.sigma_);
  xx[agp.N_] = xx_n;
  unsigned int savedIndex = 0;

  for (/* not unsigned! */int m = 1; m < int(agp.N_); ++m) {
    const CMP agm = asiangamma(agp.delta_ * (n + agp.offset_ - agp.N_) + c_I * agp.af_,
			       agp.al_ + CMP(0, m * PI / agp.h_),
			       agp.r_,
			       agp.sigma_);
    xx[agp.N_ + m] = pow(-1, m) * agm;

    const CMP agmm = asiangamma(agp.delta_ * (n + agp.offset_ - agp.N_) + c_I * agp.af_,
				agp.al_ + CMP(0, -m * PI / agp.h_),
				agp.r_,
				agp.sigma_);
    xx[agp.N_ - m] = pow(-1, -m) * agmm;

    if (m >= int(agp.inner_loop_bound_)) {
      savedIndex = (unsigned int)m;
      break;
    }
  }
  assert(!!savedIndex xor (agp.N_ == agp.inner_loop_bound_));
  assert(savedIndex == 0 or savedIndex == agp.inner_loop_bound_);

  if (!savedIndex) {
    const CMP agN = asiangamma(agp.delta_*(n + agp.offset_ - agp.N_) + c_I * agp.af_,
			       agp.al_ + CMP(0, -int(agp.N_) * PI / agp.h_),
			       agp.r_,
			       agp.sigma_);
    xx[0] = pow(-1, -int(agp.N_)) * agN;
    savedIndex = agp.N_;
  }

  xxRet.clear();
  std::copy(xx.begin() + agp.N_ - savedIndex, xx.begin() + agp.N_ + savedIndex,
	    std::back_inserter(xxRet));

  return savedIndex;
}

CMP AsianGammaPricer::exp_factor_1_(double k) const
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  return exp(CMP(0, agp.delta_ * agp.N_ * k));
}

double AsianGammaPricer::exp_factor_2_(double k) const 
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  return exp(agp.af_ * k + agp.al_ * agp.h_) / (4.0 * PI * agp.h_);
}
