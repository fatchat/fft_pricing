#include <cmath>
#include <cassert>
#include <gsl/gsl_cdf.h>
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>

#include "pricing/eurooption.h"

//#define DEBUG_YC
//#define DEBUG_PRICING
//#define DEBUG_NORMAL

#if defined(DEBUG_YC) || defined(DEBUG_PRICING) || defined(DEBUG_NORMAL)
#include <iostream>
#endif

namespace Pricing {

  double YieldCurve::discountFactor(const Time& startTime,
				    const Time& endTime,
				    double* returnRate) const
    {
      const double rate = rateBtw(startTime, endTime);
      const double f = (endTime - startTime) * rate;
      const double discount_factor = exp(-f);
#ifdef DEBUG_YC
      std::cout << "DF: " << startTime << ", "
		<< endTime << ", "
		<< (endTime - startTime) << ", "
		<< rate << ", "
		<< f << ", "
		<< discount_factor << ", "
		<< returnRate
		<< std::endl;
#endif
      if (returnRate)
	*returnRate = rate;
#ifdef DEBUG_YC
      std::cout << "returning from DiscountFactor\n";
#endif
      return discount_factor;
    }

  double Pricer::eurocall(const OptionData& optiondata,
			  const YieldCurve& yc,
			  const Volatility& volatility,
			  const Time& now,
			  double underlyingPrice)
  {
    const double s_over_k = underlyingPrice / optiondata.strike();
    assert(s_over_k > 0);
    const double time_to_maturity = optiondata.maturity() - now;
    assert(time_to_maturity >= 0);
    double rate = 0;
    const double discount_factor = yc.discountFactor(now, optiondata.maturity(), &rate);
    const double sqrt_t = sqrt(time_to_maturity);
    const double vol = volatility.getVolatility();
    assert(vol > 0);

    const double sigma_root_t = vol * sqrt_t;
    const double d_1 = log(s_over_k) / sigma_root_t + rate * sqrt_t / vol + sigma_root_t / 2.0;
    const double d_2 = d_1 - sigma_root_t;

    const double N_d_1 = Normal(d_1);
    const double N_d_2 = Normal(d_2);

    const double result_over_k = s_over_k * N_d_1 - discount_factor * N_d_2;
    const double result = result_over_k * optiondata.strike();

#ifdef DEBUG_PRICING
    std::cout << "eurocall: "
	   << s_over_k << ", "
	   << time_to_maturity << ", "
	   << rate << ", "
	   << discount_factor << ", "
	   << sqrt_t << ", "
	   << vol << ", "
	   << sigma_root_t << ", "
	   << d_1 << ", "
	   << d_2 << ", "
	   << N_d_1 << ", "
	   << N_d_2 << ", "
	   << result_over_k << ", "
	   << result
	   << std::endl;
#endif
    return result;
  }

  double Normal(double x)
  {
    const double result = gsl_cdf_ugaussian_P(x);
    return result;
  }

} // namespace
