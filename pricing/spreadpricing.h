#ifndef SPREADPRICING_H
#define SPREADPRICING_H

#include "csutils/globals.h"

class SpreadPricing {
  // provided
  const double S1_;
  const double S2_;
  const double K_;
  const double sigma1_;
  const double sigma2_;
  const double rho_;
  const double r_;
  const double time_;

  // cached
  const double T1_;
  const double T2_;
  const double X1_;
  const double X2_;
  const double sigma1sq_;
  const double sigma2sq_;
  const double sigma12rho_;
  const CMP v1_;
  const CMP v2_;

  CMP spreadgamma_(CMP u1, CMP u2) const;

 public:
  SpreadPricing(double S1, double S2, double K, double sigma1,
		double sigma2, double rho, double r, double time);
  CMP spreadpricing(double e1, double e2, int N) const;
};

#endif // SPREADPRICING_H
