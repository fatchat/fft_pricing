#include <cmath>
#include <string>
#include <iostream>

#include "csutils/configfile.h"
#include "csutils/globals.h"
#include "csutils/key_value_table.h"
#include "mathutils/interpolate.h"
#include "pricing/agpdata.h"

unsigned int
get_inner_loop_bound(double sigma, double time, unsigned int N)
{
  const std::string vol_ilb_filename
    = get_value_from_config_file<std::string>("VOL_ILB_FILE", "");
  const InterpolatedKVT kvt(vol_ilb_filename);

  const double key = sigma * sqrt(time);
  double retVal;

  if (!kvt.getVal(key, retVal)) {
    retVal
      = get_value_from_config_file("INNER_LOOP_BOUND", N);
  }
  retVal = std::min(retVal, (double)N);
  retVal = std::max(retVal, 0.0);
#if 0
  std::cout << "KEYS ";
  std::copy(kvt.kvt().keys().begin(), kvt.kvt().keys().end(),
  	    std::ostream_iterator<double>(std::cout, "; "));
  std::cout << "\nreturning " << retVal << std::endl;
#endif
  return (unsigned int)retVal;
}

AgpData::AgpData(double S, double r, double sigma, double time, double K,
		     double A, unsigned int N)
  : S_(S), r_(r), sigma_(sigma), time_(time), K_(K), N_(N), A_(A)
  , offset_(get_value_from_config_file("offset", 0.5))
  , inner_loop_bound_(get_inner_loop_bound(sigma_, time_, N_))
  , h_(sigma_ * sigma_ * time_ / 4.)
  , k_(scale_strike(K_))
  , af_(get_value_from_config_file("af", A / (2 * k_)))
  , al_(get_value_from_config_file("al", A / (2 * h_)))
  , delta_(get_value_from_config_file("delta", fabs(PI/k_)))
  , finalExpFactor_(exp(-r_ * time_) / time_ * S_ * delta_)
{
  assert(inner_loop_bound_ <= N_);
  assert(inner_loop_bound_ > 0);
}

double AgpData::scale_strike(double K) const
{ return log(K * h_ / S_); }

double AgpData::unscale_strike(double k) const
{ return exp(k) / h_ * S_; }

void
AgpData::show_params(std::ostream *os) const
{
  if (os) {
    *os << "S: " << S_
	<< " r: " << r_
	<< " sigma: " << sigma_
	<< " time: " << time_
	<< " K: " << K_
	<< " af: " << af_
	<< " al: " << al_
	<< " N:" << N_
	<< " delta: " << delta_
	<< " h: " << h_
	<< " k: " << k_
	<< " ILB: " << inner_loop_bound_
	<< " offset: " << offset_
	<< std::endl;
  }
}

std::vector<double>
AgpData::create_log_strikes() const
{
  std::vector<double> k(2 * N_);
  k[0] = k_;
  const double lambda = PI / delta_ / N_;
  for (unsigned int j = 1; j < 2 * N_; ++j) {
    k[j] = k[j - 1] + lambda;
  }
  return k;
}

CMP AgpData::f_1(double k) const
{
  return exp(CMP(0, delta_ * N_ * k));
}

double AgpData::f_2(double k) const
{
  return exp(af_ * k + al_ * h_) / (4.0 * PI * h_);
}

CMP AgpData::f_3(int j) const
{
  return exp(CMP(0, -PI * j * offset_ / N_));
}

CMP AgpData::f_4(int n) const
{
  return exp(CMP(0, -delta_ * (n + offset_) * k_));
}
