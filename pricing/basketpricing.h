#ifndef BASKETPRICING_H
#define BASKETPRICING_H

#include <iosfwd>
#include "csutils/globals.h"

class BasketData {

 public:
  // provided
  const double S1_;
  const double S2_;
  const double K_;
  const double sigma1_;
  const double sigma2_;
  const double rho_;
  const double r_;
  const double time_;

  // computed
  const double T1_;
  const double T2_;
  const double X1_;
  const double X2_;
  const double v0_;
  const double v1_;

  BasketData(double S1, double S2, double K, double sigma1, double sigma2,
	     double rho, double r, double time);
  void show_params(std::ostream*) const;
 private:
  // no impl
  BasketData();
  BasketData& operator=(const BasketData&);
  // default impl
  // BasketData(const BasketData&);
};

BasketData create_basketdata();

class BasketPricing {
  CMP basketgamma_(CMP u1, CMP u2, CMP (*p_hat)(CMP, CMP)) const;
 private:
  const BasketData bd_;
  const double delta_;
 public:
  BasketPricing(const BasketData&, double delta);
  CMP integral_(double e1, double e2, int N, CMP (*p_hat)(CMP, CMP)) const;
  CMP basketpricing(double e1, double e2, int N) const;
  CMP spreadpricing(double e1, double e2, int N) const;
  const BasketData& basketData() const { return bd_; }
 private:
  // no impl
  BasketPricing();
  BasketPricing(const BasketPricing&);
  BasketPricing& operator=(const BasketPricing&);
 private:
  double Kdisc_() const { return exp(-bd_.r_ * bd_.time_) * bd_.K_; }
  double factor_() const { return Kdisc_() * delta_ * delta_ / (4 * PI * PI); }
};

#endif // BASKETPRICING_H

