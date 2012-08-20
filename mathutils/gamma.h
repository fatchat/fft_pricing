#ifndef GAMMA_H
#define GAMMA_H

#include "csutils/utils.h"
#include "csutils/globals.h"

class Gamma {

  static const double p[];
  static const int G_;
  
 public:
  static CMP gamma(CMP);
  static CMP gsl_gamma(CMP);
  static CMP log_gsl_gamma(CMP);
};

#endif //GAMMA_H
