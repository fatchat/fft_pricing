#include "csutils/key_value_table.h"
#include "mathutils/interpolate.h"
#include <utility>

bool InterpolatedKVT::getVal(double key, double& value) const
{
  std::pair<bool, double> result
    = MathUtils::interpolate(kvt_.keys(), kvt_.values(), key);
  if (result.first) {
    value = result.second;
    return true;
  }
  return false;
}

double InterpolatedKVT::operator[] (double key) const
{
  return MathUtils::interpolate(kvt_.keys(), kvt_.values(), key).second;
}
