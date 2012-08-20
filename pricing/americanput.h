#ifndef AMERICANPUT_H
#define AMERICANPUT_H

#include <iosfwd>

class AmericanPut {

  const double S_;
  const double K_;
  const double r_;
  const double sigma_;
  const double time_;
  const unsigned int N_;

  const double gamma_;
  const double a_;
  const double b_;
  const double c_;
  const double tau_;

 private:
  double calc_gamma_() const;
  double log_factor_(double) const;
  double exp_factor_(double) const;
  double megan_fox_(double, double, double) const;
  double amitabh_(double) const;
  double bacchan_(double) const;
  double sholay_(double, double) const;
  double f1_(double) const;
  double f2_(double) const;
  double finite_time_price_integrand_(double) const;
  double optimal_exercise_integrand_(double) const;

 public:
  AmericanPut(double S, double K, double r, double sigma, double time, unsigned int N)
    : S_(S)
    , K_(K)
    , r_(r)
    , sigma_(sigma)
    , time_(time)
    , N_(N)
    , gamma_(calc_gamma_())
    , a_((1 + gamma_) / 2.0)
    , b_((1 - gamma_) / 2.0)
    , c_(1 + b_ / gamma_)
    , tau_(time_ * pow(sigma_, 2) / 2.0)
  {}

  void graph(std::ostream&) const;
  void show_cached_vals(std::ostream&) const;

  double perpetual_price() const;
  double finite_time_price() const;
  double perpetual_exercise() const;
  double optimal_exercise() const;
};

#endif // AMERICANPUT_H
