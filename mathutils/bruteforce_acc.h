#ifndef BRUTEFORCE_ACC_H
#define BRUTEFORCE_ACC_H

#include <vector>
#include <numeric>

template<class T>
class BruteForce {
  std::vector<T> series_;
 public:
  BruteForce(const std::vector<T>& series)
    : series_(series) { }
  T sum() const
  { return std::accumulate(series_.begin(), series_.end(), T(0)); }
};

#endif // BRUTEFORCE_ACC_H
