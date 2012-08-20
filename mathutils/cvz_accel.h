#ifndef CVZ_ACCEL_H
#define CVZ_ACCEL_H

#include <vector>
#include <cmath>

#ifdef CVZ_DEBUG
#include <iostream>
#endif

#include "csutils/globals.h"

template<class T>
class CVZAccelerator {
  double d_;
  double b_;
  double c_;
  T s_;
 public:
  CVZAccelerator(const std::vector<T>& summands)
    {
      const int sz = int(summands.size());
      d_ = pow(3 + sqrt(8), sz);
      d_ = (d_ + 1. / d_) / 2.0;
      b_ = -1.;
      c_ = -d_;
      s_ = 0;  
#ifdef CVZ_DEBUG
      std::cout << "d:" << d_
		<< " b:" << b_
		<< " c:" << c_
		<< " sz:" << sz
		<< std::endl;
#endif
      for (int k = 0; k < sz; ++k) {
	c_ = b_ - c_;
	s_ = s_ + c_ * summands[k] * pow(-1.0, k);
	b_ *= (k + sz) * (k - sz) / (k + 0.5) / (k + 1);
#ifdef CVZ_DEBUG
	std::cout << "k:" << k
		  << " b:" << b_
		  << " c:" << c_
		  << " s:" << s_
		  << std::endl;
#endif
      }
    }

  T sum() const { return s_ / d_; }
};

#endif // CVZ_ACCEL_H
