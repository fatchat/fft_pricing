#ifndef EULER_ACCELERATOR_H
#define EULER_ACCELERATOR_H

#include <vector>
#include "csutils/globals.h"

#ifndef EULER_ACCELERATOR_ACCURACY
#define EULER_ACCELERATOR_ACCURACY 1.0e-04
#endif

template<class T>
class EulerAccelerator {

  std::vector<T> wksp_;
  unsigned int n_;
  unsigned int ncv_;
  T sum_;
  double eps_;
  T lastval_;
  double lasteps_;
  std::vector<T> series_;

 public:
   EulerAccelerator(const std::vector<T>& series)
    : wksp_(series.size())
    , n_(0)
    , ncv_(0)
    , sum_(T(0))
    , eps_(EULER_ACCELERATOR_ACCURACY)
    , lastval_(T(0))
    , lasteps_(0.0)
    , series_(series)
    {}

  bool converged() const { return ncv_ >= 2; }
  T sum();

 private:
  void calc_next_(T);
};

#include "mathutils/euler_acc.hpp"

#endif // EULER_ACCELERATOR_H
