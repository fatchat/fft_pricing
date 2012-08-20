#ifndef SUMMERS_H
#define SUMMERS_H

#include <vector>

#include "csutils/globals.h"

class GenericSum {
  std::vector<CMP> series_;
 public:
  GenericSum(const std::vector<CMP>& _series)
    : series_(_series) { }
  const std::vector<CMP> series() const { return series_; }
  std::vector<CMP>& seriesRef() { return series_; }
};

template<class Accelerator>
class ComplexSum {
  GenericSum genericSum_;
 public:
  ComplexSum(const std::vector<CMP>&, double);
  CMP sum();
};

template<class Accelerator>
class RealSum {
  GenericSum genericSum_;
  std::vector<double> real_series_;
  std::vector<double> imag_series_;
 public:
  RealSum(const std::vector<CMP>&, double);
  const std::vector<double>& real_series() const { return real_series_; }
  const std::vector<double>& imag_series() const { return imag_series_; }
  CMP sum();
};

class PosNeg {
  std::vector<double> pos_;
  std::vector<double> neg_;
 public:
  PosNeg(const std::vector<double>&);
  const std::vector<double>& pos() const { return pos_; }
  const std::vector<double>& neg() const { return neg_; }
};

template<class Accelerator>
class PosNegSum : public RealSum<Accelerator>
{
  PosNeg real_;
  PosNeg imag_;
 public:
  PosNegSum(const std::vector<CMP>&, double);
  const PosNeg& real() const { return real_; }
  const PosNeg& imag() const { return imag_; }
  CMP sum();
};

template<class Accelerator>
class CombinedRealSum : public PosNegSum<Accelerator>
{
  std::vector<double> real_combined_;
  std::vector<double> imag_combined_;
 public:
  CombinedRealSum(const std::vector<CMP>&, double);
  const std::vector<double> real_combined() const { return real_combined_; }
  const std::vector<double> imag_combined() const { return imag_combined_; }
  CMP sum();
};

#include "mathutils/summers.hpp"

#endif // SUMMERS_H
