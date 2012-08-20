#include <cassert>
#include <algorithm>

#include "mathutils/interpolate.h"

namespace MathUtils {

double
linear_interp_(double left_x, double right_x,
	       double left_y, double right_y,
	       double x)
{
  assert (left_x <= x);
  assert (x <= right_x);
  const double y = left_y +
    (right_y - left_y) / (right_x - left_x)
    * (x - left_x);
  return y;
}

std::pair<bool, double>
interpolate(const std::vector<double>& x_vals,
	    const std::vector<double>& y_vals,
	    double searchFor)
{
  assert(x_vals.size() == y_vals.size());
  const int N = x_vals.size();
  if (N == 0) {
    return std::make_pair<bool, double>(false, 0.0);
  }
  const std::pair<const double*, const double*>
    x_range = std::equal_range(&x_vals[0],
			       &x_vals[N],
			       searchFor);

  if (*(x_range.first) == searchFor) {
    const int index = x_range.first - &x_vals[0];
    return std::make_pair<bool, double>(true, y_vals[index]);
  }
  else if (x_range.first != &x_vals[0] and x_range.second != &x_vals[N]) {
    const int lindex = (x_range.first - 1) - &x_vals[0];
    const int rindex = x_range.second - &x_vals[0];
    assert (rindex - lindex == 1);

    const double left_x_val = x_vals[lindex];
    const double right_x_val = x_vals[rindex];

    const double left_y_val = y_vals[lindex];
    const double right_y_val = y_vals[rindex];

    const double retVal = linear_interp_(left_x_val, right_x_val,
					 left_y_val, right_y_val,
					 searchFor);
    return std::make_pair<bool, double>(true, retVal);
  }
  else {
    return std::make_pair<bool, double>(false, 0.0);
  }
}

} // namespace
