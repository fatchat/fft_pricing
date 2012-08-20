#include <iostream>
#include <numeric>

#include "mathutils/aitken_acc.h"

int main()
{
  std::vector<double> a;

  for(double ii = 0; ii < 105.; ++ii) {
    a.push_back(1.0 / (ii + 1) / (ii + 1));
  }

  std::cout << "bf sum= ";
  std::cout << std::accumulate(a.begin(), a.end(), 0.0) << std::endl;

  AitkenAccelerator<double> acc(a);
  std::cout << acc.sum() << std::endl;

  return 0;
}
