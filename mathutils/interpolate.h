#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <utility>
#include <vector>

namespace MathUtils {

double
linear_interp_(double left_x, double right_x,
	       double left_y, double right_y,
	       double x);

std::pair<bool, double>
interpolate(const std::vector<double>& x_vals,
	    const std::vector<double>& y_vals,
	    double searchFor);

}

#endif // INTERPOLATE_H
