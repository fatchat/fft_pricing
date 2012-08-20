#ifndef LEVINU_ACC_H
#define LEVINU_ACC_H

#include <vector>
#include <gsl/gsl_sum.h>

class LevinUAccelerator {

  std::vector<double> series_;
  gsl_sum_levin_u_workspace *workspace_;

 public:
  LevinUAccelerator(const std::vector<double>& series);
  ~LevinUAccelerator();
  double sum();
};

#endif // LEVINU_ACC_H
