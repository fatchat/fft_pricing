#include <numeric>

#include "pricing/parity.h"
#include "mathutils/fft.h"
#include "csutils/configfile.h"
#include "csutils/utils.h"
#include "csutils/sequtils.h"

template<class InnerLoopSummer>		   
void AsianGammaPricer::create_little_x_(std::vector<CMP>& little_x) const
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  little_x.clear();
  const double small_bound
    = get_value_from_config_file<double>("small_bound", -1.0);

  for(unsigned int n = 0; n < 2 * agp.N_; ++n) {
    std::vector<CMP> truncated_xx;
    create_inner_loop_summands_(truncated_xx, n);
#if 0
    std::vector<double> real, imag;
    SeqUtils::split_real_imag(truncated_xx, real, imag);
    show_vector(real, std::string("REAL n= ") + toString(n), &std::cout);
    show_vector(imag, std::string("IMAG n= ") + toString(n), &std::cout);
#endif
    const CMP xx_n = InnerLoopSummer(truncated_xx, small_bound).sum();
    const CMP expFactor_1_1 = agp.f_4(n);
    const CMP scaled_xx_n = xx_n * expFactor_1_1;

    little_x.push_back(scaled_xx_n);
  }
}

template<class InnerLoopSummer>
CMP AsianGammaPricer::fastasianpricing_() const
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  std::vector<CMP> little_x;
  create_little_x_<InnerLoopSummer>(little_x);
  CMP sum = std::accumulate(little_x.begin(), little_x.end(), CMP(0,0));
  const CMP mult_sum_by = agp.f_1(agp.k_) * agp.f_2(agp.k_) * agp.finalExpFactor_;
  sum *= mult_sum_by;
  return sum;
}

template<class InnerLoopSummer>
std::vector<CMP>
AsianGammaPricer::fastasianpricingFFT_(const std::vector<double> *logStrikes) const
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  std::vector<CMP> little_x;
  create_little_x_<InnerLoopSummer>(little_x);

  const unsigned int P = 2 * agp.N_;
  FFTCalc fftCalc(P);

  fftCalc.setData(little_x);
  assert(!fftCalc.performFFT());

  std::vector<CMP> big_X;
  fftCalc.getData(big_X);

  // the following check passes
  //  const CMP sum = std::accumulate(little_x.begin(), little_x.end(), CMP(0,0));
  //  std::cout << "FFT X_0=" << big_X[0] << " sum=" << sum << std::endl;

  const std::vector<double> k
    = logStrikes ? *logStrikes : agp.create_log_strikes();
  std::vector<CMP> Y(P);
  for (unsigned int j = 0; j < P; ++j) {
    const CMP    expFactor_1 = agp.f_1(k[j]);
    const double expFactor_2 = agp.f_2(k[j]);
    const CMP    expFactor_3 = agp.f_3(j);
    Y[j] = big_X[j] * expFactor_1 * expFactor_2 * expFactor_3;
  }

  std::transform(Y.begin(), Y.end(), Y.begin(),
		 std::bind2nd(std::multiplies<CMP>(), agp.finalExpFactor_));
  return Y;
}

template<class InnerLoopSummer>
void AsianGammaPricer::calcCallPutPrices()
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  asianCallPrice_ = fastasianpricing_<InnerLoopSummer>().real();
  asianPutPrice_ = asianCallPrice_ + fixedstrikeparity(agp.S_, agp.K_, agp.r_, agp.time_);
}

template<class InnerLoopSummer>
void AsianGammaPricer::calcFloatingCallPutPrices()
{
  assert(agp_data_.get());
  const AgpData& agp = *agp_data_;
  const double FSP_1 = fixedstrikeparity(0, 1, -agp.r_, agp.time_);
  // negate r_ and set strike to stock price
  boost::shared_ptr<AgpData> newData(new AgpData(agp.S_, -agp.r_, agp.sigma_,
						 agp.time_, agp.S_, agp.A_,
						 agp.N_));
  const boost::shared_ptr<AgpData> oldData = agp_data_;
  newParams(newData, false);
  asianFloatPutPrice_ = fastasianpricing_<InnerLoopSummer>().real() * FSP_1;
  newParams(oldData, false);

  const double FSP_2 = fixedstrikeparity(0, 1, -agp.r_, agp.time_);
  const double FSP_3 = fixedstrikeparity(agp.S_, agp.K_, -agp.r_, agp.time_);
  asianFloatCallPrice_ = asianFloatPutPrice_ + FSP_2 * FSP_3;
}
