#include <cmath>
#include "pricing/parity.h"

double fixedstrikeparity(double S, double K, double r, double time)
{
  const double fsp =
    exp(-r * time) * (K + S / (r * time)) - S / (r * time);
  return fsp;
}

