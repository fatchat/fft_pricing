#include <cmath>
#include <iostream>
#include <boost/format.hpp>

#include "pricing/basketpricing.h"
#include "mathutils/gamma.h"
#include "csutils/globals.h"
#include "csutils/utils.h"
#include "csutils/logger.h"
#include "csutils/configfile.h"

// ==========================================================================
BasketData::BasketData(double S1, double S2, double K, 
		       double sigma1, double sigma2,
		       double rho, double r, double time)
  : S1_(S1)
  , S2_(S2)
  , K_(K)
  , sigma1_(sigma1)
  , sigma2_(sigma2)
  , rho_(rho)
  , r_(r)
  , time_(time)
  , T1_(S1_/K_)
  , T2_(S2_/K_)
  , X1_(log(T1_))
  , X2_(log(T2_))
    // dividend rates will appear subtracted from r_ in the v's below
  , v0_((r_ - 0.5 * pow(sigma1_, 2)) * time_)
  , v1_((r_ - 0.5 * pow(sigma2_, 2)) * time_)
{ }

// ==========================================================================
void BasketData::show_params(std::ostream* os) const
{
  if (os) {
    boost::format formatter("S: [%1%, %2%], K: %3%, sigma: [%4%, %5%], rho: %6%, r: %7%, time: %8%, T: [%9%, %10%], X: [%11%, %12%], v: [%13%, %14%]");
    formatter % S1_ % S2_ % K_ % sigma1_ % sigma2_ % rho_ % r_ % time_
	      % T1_ % T2_ % X1_ % X2_ % v0_ % v1_;
    *os << formatter.str();
  }
}

// ==========================================================================
BasketData create_basketdata()
{
  const double S1 = get_value_from_config_file("S_0", 100.);
  const double S2 = get_value_from_config_file("S_1", 100.);
  const double K = get_value_from_config_file("K", 200.);
  const double sigma1 = get_value_from_config_file("sigma_0", 0.05);
  const double sigma2 = get_value_from_config_file("sigma_1", 0.05);
  const double rho = get_value_from_config_file("rho_0_1", 0.5);
  const double r = get_value_from_config_file("r", 0.09);
  const double time = get_value_from_config_file("time", 1.);

  return BasketData(S1, S2, K, sigma1, sigma2, rho, r, time);
}

// ==========================================================================
BasketPricing::BasketPricing(const BasketData& basketData, double delta)
  : bd_(basketData)
  , delta_(delta)
{ }

// ==========================================================================
CMP p_hat_basket(CMP u1, CMP u2)
{
  const CMP log_num = Gamma::log_gsl_gamma(-c_I * u1)
                    + Gamma::log_gsl_gamma(-c_I * u2);
  const CMP log_den = Gamma::log_gsl_gamma(-c_I * (u1+u2) + CMP(2,0));
  return exp(log_num - log_den); 
}

CMP p_hat_spread(CMP u1, CMP u2)
{
  const CMP log_num_1 = Gamma::log_gsl_gamma(c_I * u1 + c_I * u2 - 1.);
  const CMP log_num_2 = Gamma::log_gsl_gamma(-c_I * u2);
  const CMP log_den = Gamma::log_gsl_gamma(c_I * u1 + 1.);
  return exp(log_num_1 + log_num_2 - log_den);
}

// ==========================================================================
CMP
BasketPricing::basketgamma_(CMP u1, CMP u2, CMP (*p_hat)(CMP, CMP)) const
{
  const CMP fs = u1 * bd_.v0_ + u2 * bd_.v1_;
  const CMP a = c_I * fs;
  const CMP b = 0.5 * bd_.time_
    * (pow(u1 * bd_.sigma1_, 2) + pow(u2 * bd_.sigma2_, 2)
       + 2 * bd_.sigma1_ * bd_.sigma2_ * bd_.rho_ * u1 * u2);
  const CMP phi = exp(a - b);
  const CMP ss = u1 * bd_.X1_ + u2 * bd_.X2_;
  const CMP psi = exp(c_I * ss) * phi;
  const CMP phat = (*p_hat)(u1, u2);
  const CMP retVal = psi * phat;
#if 0
  {
    boost::format formatter("fs: %1%, a: %2%, b: %3%, phi: %4%, ss: %5%, phat: %6%, retval: %7%");
    formatter % fs % a % b % phi % ss % phat % retVal;
    logthis(formatter.str());
  }
#endif
  return retVal;
}

// ==========================================================================
CMP BasketPricing::integral_(double e1, double e2, int N,
			     CMP (*p_hat)(CMP, CMP)) const
{
  CMP sum(0,0);
  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      const CMP bg = basketgamma_(delta_ * (i - 0.5 * N) + c_I * e1,
				  delta_ * (j - 0.5 * N) + c_I * e2,
				  p_hat);
#if 0
      LOG("i=" << i << " j=" << j << " bg=" << bg);
#endif
      sum += bg;
    }
  }
 #if 0
  LOG("sum= " << sum);
#endif
  return sum;
}

// ==========================================================================
CMP BasketPricing::spreadpricing(double e1, double e2, int N) const
{
  assert(e1 + e2 < 0);
  assert(e2 > 0);

  const CMP sum = integral_(e1, e2, N, &p_hat_spread);
  const CMP retVal = sum * factor_();

  return retVal;
}

// ==========================================================================
CMP BasketPricing::basketpricing(double e1, double e2, int N) const
{
  assert(e1 > 0);
  assert(e2 > 0);

  const CMP sum = integral_(e1, e2, N, &p_hat_basket);
  const double S_sum = (bd_.T1_ + bd_.T2_) * bd_.K_;
  const CMP retVal = S_sum - Kdisc_() + sum * factor_();

  return retVal;
}
