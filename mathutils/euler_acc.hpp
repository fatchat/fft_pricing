#include <cmath>
#include <cassert>

#ifdef DEBUG_EULER_ACC
#include <iostream>
#endif

template<class T>
void EulerAccelerator<T>::calc_next_(T term)
{
  assert(n_ < wksp_.size());

  if (n_ == 0) {
    wksp_[0] = term;
    n_ = 1;
    sum_ = 0.5 * term;
  }
  else {
    T tmp = wksp_[0];
    wksp_[0] = term;
    for (unsigned int j = 1; j < n_; ++j) {
      T dum = wksp_[j]; 
      wksp_[j] = 0.5 * (wksp_[j-1] + tmp);
      tmp = dum;
    }
    wksp_[n_] = 0.5 * (wksp_[n_-1] + tmp);
    if (abs(wksp_[n_]) <= abs(wksp_[n_-1])) {
      sum_ += (0.5 * wksp_[n_]);
      ++n_;
    }
    else {
      sum_ += wksp_[n_];
    }
  }
  lasteps_ = abs(sum_ - lastval_);
#ifdef DEBUG_EULER_ACC
  std::cout << "LASTEPS " << n_ << ' ' << lasteps_ << std::endl;
#endif
  if (lasteps_ <= eps_) {
    ++ncv_;
  }
  lastval_ = sum_;
}

template<class T>
T EulerAccelerator<T>::sum()
{
  for (unsigned int ii = 0; ii < series_.size(); ++ii) {
    calc_next_(series_[ii]);
    if (converged()) {
      //break
      ;
    }
  }
  return sum_;
}

