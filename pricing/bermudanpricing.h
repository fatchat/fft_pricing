#ifndef BERMUDANPRICING_H
#define BERMUDANPRICING_H

#include <vector>
#include "csutils/globals.h"

class BermudanPricing {
 public:
  // logarithmic coordinates y=log(S)-log(K)
  static double callpayoff(double y);
  static double putpayoff(double y);
  
  //characteristic function of the log process
  static CMP charfunction(double u, double r, double sigma, double time, int M);

  static std::vector<double> 
    bermudanpricing(double S, double K, double r, double sigma,
		    double time, double a, int nExer, int fftSz);
};

#endif // BERMUDANPRICING_H
