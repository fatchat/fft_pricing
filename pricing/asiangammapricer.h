#ifndef ASIANGAMMAPRICER_H
#define ASIANGAMMAPRICER_H

#include <vector>
#include <boost/smart_ptr.hpp>

#include "csutils/globals.h"
#include "pricing/agpdata.h"

class AsianGammaPricer {

  static CMP asiangamma(CMP csi, CMP lambda, double r, double sigma);

 public:
  template<class InnerLoopSummer>
    CMP fastasianpricing_() const ;

 public:
  template<class InnerLoopSummer>
    std::vector<CMP> fastasianpricingFFT_(const std::vector<double>* = 0) const;

 private:
  CMP exp_factor_1_(double) const;
  double exp_factor_2_(double) const;
  template<class InnerLoopSummer>
    void create_little_x_(std::vector<CMP>&) const;
  unsigned int create_inner_loop_summands_(std::vector<CMP>&,
					   unsigned int) const;
 private:
  boost::shared_ptr<AgpData> agp_data_;
  double asianPutPrice_;
  double asianCallPrice_;
  double asianFloatCallPrice_;
  double asianFloatPutPrice_;

  AsianGammaPricer(const AsianGammaPricer&);
  AsianGammaPricer& operator=(const AsianGammaPricer&);

 public:
  AsianGammaPricer(boost::shared_ptr<AgpData>);

  void newParams(boost::shared_ptr<AgpData>, bool reset = true);
  const AgpData* agpData() const { return agp_data_.get(); }

  template<class InnerLoopSummer>
    void calcCallPutPrices();

  template<class InnerLoopSummer>
    void calcFloatingCallPutPrices();

  double asianCallPrice() const { return asianCallPrice_; }
  double asianPutPrice() const { return asianPutPrice_; }
  double asianFloatPutPrice() const { return asianFloatPutPrice_; }
  double asianFloatCallPrice() const { return asianFloatCallPrice_; }
};

#include "pricing/asiangammapricer.hpp"

#endif //ASIANGAMMAPRICER_H
