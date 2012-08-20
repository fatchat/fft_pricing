#include <cmath>
#include <iostream>

#include "pricing/americanput.h"
#include "csutils/globals.h"
#include "csutils/utils.h"
#include "mathutils/gauss_legendre.h"

double sq_sum(double a, double b) { return a * a + b * b; }

double AmericanPut::calc_gamma_() const
{
  check_nz(sigma_, "AmericanPut::sigma_");
  check_nz(r_, "AmericanPut::r_");
  const double retVal = 2 * r_ / pow(sigma_, 2);
  return retVal;
}

void AmericanPut::show_cached_vals(std::ostream& os) const
{
  os << "#S=" << S_
     << " K=" << K_
     << " r_=" << r_
     << " sigma=" << sigma_
     << " time_=" << time_
     << " N=" << N_
     << " gamma=" << gamma_
     << " a=" << a_
     << " b=" << b_
     << " c=" << c_
     << " tau=" << tau_
     << std::endl;
}

double AmericanPut::log_factor_(double x) const
{
  const double retVal = log( sqrt(sq_sum(a_, x)) / gamma_ );
  return retVal;
}

double AmericanPut::exp_factor_(double x) const
{
  const double retVal = exp(-tau_ * x * x);
  return retVal;
}

double AmericanPut::megan_fox_(double z, double x, double y) const
{
  const double log_term = log_factor_(z);
  check_nz(a_, "AmericanPut::a_");
  const double arc_tan = atan(z / a_);
  const double top = x * log_term + y * arc_tan;
  const double bottom = sq_sum(b_, z);
  check_nz(bottom, "AmericanPut::megan_fox_()::bottom");
  const double retVal = top / bottom;
  return retVal;
}

double AmericanPut::amitabh_(double x) const
{
  const double retVal = megan_fox_(x, b_ * b_ - x * x, 2 * b_ * x);
  return retVal;
}

double AmericanPut::bacchan_(double x) const
{
  const double retVal = megan_fox_(x, 2 * b_ * x, -(b_ * b_ - x * x));
  return retVal;
}

double AmericanPut::sholay_(double SoverK, double x) const
{
  const double logS = log(SoverK);
  const double xlogS = x * logS;
  const double ba = bacchan_(x);
  const double arg = ba + xlogS;

  const double ab = amitabh_(x);
  const double first = exp(ab) * (b_ * cos(arg) + x * sin(arg) );

  const double second = -x * sin(xlogS)
    - (b_ * c_ + pow(x, 2) / gamma_) * cos(xlogS);

  const double retVal = first + second;
  return retVal;
}

double AmericanPut::perpetual_price() const
{
  const double top = pow(K_, 1 + gamma_) * pow(gamma_, gamma_);
  const double bottom = pow(1 + gamma_, 1 + gamma_) * pow(S_, gamma_);
  check_nz(bottom, "AmericanPut::perpetual_price()::bottom");
  const double retVal = top / bottom;
  return retVal;
}

double AmericanPut::finite_time_price_integrand_(double x) const
{
  const double retVal = exp_factor_(x) * sholay_(S_/K_, x) / sq_sum(a_, x) / sq_sum(b_, x);
  return retVal;
}

void AmericanPut::graph(std::ostream& os) const
{
  const double dx = 1.0e-4;
  const double x_start = 0.0;

  for (unsigned int ii = 0; ii < N_; ++ii) {
    const double x = x_start + ii * dx;
    const double retVal_1 = finite_time_price_integrand_(x);
    const double retVal_2 = optimal_exercise_integrand_(x);
    os << "FTPGRAPH " << x << ' ' << retVal_1 << std::endl;
    os << "OPTGRAPH " << x << ' ' << retVal_2 << std::endl;
  }
}

double AmericanPut::finite_time_price() const
{ 
  check_nz(K_, "AmericanPut::K_");
  const double SoverK = S_ / K_;

  const double dx = 1.0e-4;
  const double x_start = 0.0;
  const double x_end = x_start + N_ * dx;

  GaussLegendre<AmericanPut> gl(*this, &AmericanPut::finite_time_price_integrand_);
  const double integral = gl.integrate(4, x_start, x_end);
  std::cout << "ftp integral=" << integral << std::endl;

  const double price_diff =
    K_ * gamma_ * pow(SoverK, b_) * exp_factor_(a_) * integral / PI;

  return perpetual_price() + price_diff;
}

double AmericanPut::perpetual_exercise() const
{
  return K_ * gamma_ / (1.0 + gamma_);
}

double AmericanPut::f1_(double x) const
{
  const double retVal = megan_fox_(x, b_, x);
  return retVal;
}

double AmericanPut::f2_(double x) const
{
  const double retVal = megan_fox_(x, x, -b_);
  return retVal;
}

double AmericanPut::optimal_exercise_integrand_(double x) const
{
  const double retVal = x * exp_factor_(x) * exp(-f1_(x)) * sin(f2_(x)) / sq_sum(a_, x);
  return retVal;
}

double AmericanPut::optimal_exercise() const
{
  const double dx = 1.0e-4;
  const double x_start = 0.0;
  const double x_end = x_start + N_ * dx;

  GaussLegendre<AmericanPut> gl(*this, &AmericanPut::optimal_exercise_integrand_);
  const double integral = gl.integrate(64, x_start, x_end);
  std::cout << "optimal exercise integral=" << integral << std::endl;

  const double diff = 2.0 * exp_factor_(a_) * integral / PI * K_;

  return perpetual_exercise() + diff;
}
