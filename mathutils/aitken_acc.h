#ifndef AITKENACC_H
#define AITKENACC_H

#include <vector>
#include <cassert>

template<class T>
class AitkenAccelerator {

  std::vector<T> series_;
  
 public:
  AitkenAccelerator(const std::vector<T>& series)
    : series_(series)
  {
  }

  T sum() const;
};

#include "mathutils/aitken_acc.hpp"

#endif // AITKENACC_H
