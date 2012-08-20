#ifdef DEBUG_AITKEN
#include <iostream>
#endif

#include <numeric>
#include "csutils/utils.h"

template<class T>
T AitkenAccelerator<T>::sum() const
{
  assert(series_.size() >= 3);
  const unsigned int N = series_.size() - 2;
  const T x_n =  std::accumulate(&series_[0], &series_[N], T());
  const T x_nplus1 = std::accumulate(&series_[0], &series_[N + 1], T());
  const T x_nplus2 = std::accumulate(&series_[0], &series_[N + 2], T());

#ifdef DEBUG_AITKEN
  std::cout << "AITKEN_PARTIAL "
	    << series_.size() << ' '
	    << x_n << ' '
	    << x_nplus1 << ' '
	    << x_nplus2 << ' '
    ;
#endif

  const T numerator = x_nplus2 * x_n - x_nplus1 * x_nplus1;
  const T denominator = x_nplus2 - (x_nplus1 + x_nplus1) + x_n;

  check_nz(denominator, "AitkenAccelerator::sum()::denominator");

  return numerator / denominator;
}
