#include <vector>
#include <functional>
#include <algorithm>

#include "mathutils/summers.h"
#include "csutils/sequtils.h"

PosNeg::PosNeg(const std::vector<double>& _series)
{
  std::vector<double> non_const_series(_series);
  SeqUtils::split_positive_negative(non_const_series, pos_, neg_);
  std::reverse(pos_.begin(), pos_.end());
  std::transform(neg_.begin(), neg_.end(),
		 neg_.begin(), std::negate<double>());
}

