#ifndef AGPDATA_H
#define AGPDATA_H

class AgpData {
 public:
  // provided
  const double S_;
  const double r_;
  const double sigma_;
  const double time_;
  const double K_;
  const unsigned int N_;
  const double A_;
  const double offset_;
  const unsigned int inner_loop_bound_;

  //calculated
  const double h_;
  const double k_;
  const double af_;
  const double al_;
  const double delta_;
  const double finalExpFactor_;

 public:
  AgpData(double, double, double, double, double, double, unsigned int);
  double scale_strike(double) const;
  double unscale_strike(double) const;
  void show_params(std::ostream*) const;
  std::vector<double> create_log_strikes() const;
  CMP f_1(double) const;
  double f_2(double) const;
  CMP f_3(int) const;
  CMP f_4(int) const;
};



#endif // AGPPARAMS_H
