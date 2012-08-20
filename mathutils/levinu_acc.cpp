#include <cassert>

#include "mathutils/levinu_acc.h"

LevinUAccelerator::LevinUAccelerator(const std::vector<double>& series)
  : series_(series)
  , workspace_(0)
{
  assert (series.size());
  workspace_ = gsl_sum_levin_u_alloc(series.size());
}

LevinUAccelerator::~LevinUAccelerator()
{
  gsl_sum_levin_u_free(workspace_);
  workspace_ = 0;
}

double LevinUAccelerator::sum()
{
  double sumResult = 0.0;
  double sumError = 0.0;
  // array entries must be non-zero!
  gsl_sum_levin_u_accel(&series_[0], series_.size(),
			workspace_,
			&sumResult, &sumError);
  return sumResult;
}

