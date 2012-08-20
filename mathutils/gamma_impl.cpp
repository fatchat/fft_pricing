#include <gsl/gsl_sf_gamma.h>
#include <cassert>

#include "csutils/utils.h"
#include "mathutils/gamma.h"

#ifdef DEBUG_GAMMA
#undef DEBUG_GAMMA
#endif
// #define DEBUG_GAMMA

#ifdef DEBUG_GSL_GAMMA
#undef DEBUG_GSL_GAMMA
#endif
// #define DEBUG_GSL_GAMMA

const double Gamma::p[] =  { 0.99999999999980993,
			     676.5203681218851,
			     -1259.1392167224028,
			     771.32342877765313,
			     -176.61502916214059,
			     12.507343278686905,
			     -0.13857109526572012,
			     9.9843695780195716e-6,
			     1.5056327351493116e-7};

const int Gamma::G_ = sizeof(p)/sizeof(p[0]) - 2;

CMP Gamma::gamma(CMP z)
{
  if (real(z) < 0.5) {
    return PI / (sin(PI * z) * gamma(1.0 - z));
  }
  
  CMP_nan_check_(z, z, "z");
  z -= 1.0;
 
  CMP x = p[0];
  for (int k = 1; k < G_ + 2; k++) {
    x += p[k] / (z + CMP(k,0));
  }
    CMP_nan_check_(z, x, "x");

  CMP t = z + (G_ + 0.5);
  CMP_nan_check_(z, t, "t");

  const CMP power = pow(t, z + 0.5);
  CMP_nan_check_(z, power, "power");

  const CMP exp_t = exp(-t);
  CMP_nan_check_(z, exp_t, "exp_t");

  const CMP retVal = sqrt(2 * PI) * power * exp_t * x;
  CMP_nan_check_(z, retVal, "gamma");

#ifdef DEBUG_GAMMA
  std::cout << "# z=" << z
	    << " x=" << x
	    << " t=" << t
	    << " power=" << power
	    << " exp_t=" << exp_t
	    << " retVal=" << retVal
	    << std::endl;
#endif

  return retVal;
}

CMP Gamma::log_gsl_gamma(CMP z)
{
  gsl_sf_result lnr;
  gsl_sf_result arg;
  if (gsl_sf_lngamma_complex_e(z.real(), z.imag(), &lnr, &arg)) {
    std::cout << "#error in gsl_gamma\n";
    assert(0);
  }

  const CMP retVal = lnr.val + c_I * arg.val;

#ifdef DEBUG_GSL_GAMMA
  std::cout << "log_gsl_gamma " << z << " =" << retVal
	    << " real=" << lnr.val
	    << " imag=" << arg.val
	    << std::endl;
#endif

  return retVal;
}

CMP Gamma::gsl_gamma(CMP z)
{
  const CMP retVal = exp(log_gsl_gamma(z));
  CMP_nan_check_(z, retVal, "gsl_gamma");

  return retVal;
}
