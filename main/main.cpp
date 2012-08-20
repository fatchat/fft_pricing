#include <iostream>
#include <string>
#include <utility>

#include "pricing/fteurocalc.h"
#include "pricing/eurooption.h"
#include "csutils/get_opt.h"
#include "csutils/configfile.h"
#include "csutils/utils.h"
#include "csutils/logger.h"
#include "csutils/prog_init.h"
#include "mathutils/interpolate.h"

int main (int argc, char* argv[])
{
  Getopt getopt;
  getopt.addOption("fft", Option::NO_ARG);
  getopt.addOption("nstrikes", Option::REQ_ARG);
  prog_init(getopt, argc, argv);
  const bool fft = getopt.getOption('f').is_set();
  unsigned int computeTill = atoi(getopt.getOption('n').arg().c_str());
    
  const unsigned int N = get_value_from_config_file("N", 100);
  const double S = get_value_from_config_file("S", 100.);
  const double K = get_value_from_config_file("K", 100.);
  const double R = get_value_from_config_file("r", 0.09);
  const double sigma = get_value_from_config_file("sigma", 0.05);
  const double time = get_value_from_config_file("time", 1.);
  const double alpha = get_value_from_config_file("alpha", 1.);
  const double dpsi = get_value_from_config_file("dpsi", 0.25);

  if (computeTill == 0) {
    computeTill = 1;
  }
  if (computeTill > N) {
    std::cerr << "nstrikes shoule be less than " << N << std::endl;
    exit(1);
  }

  const Pricing::Time maturity(time);
  const Pricing::YieldCurve yc(R);
  const Pricing::Volatility vol(sigma);
  const Pricing::Time now(0);
  Pricing::Pricer pricer;

  FTEuroCalc fteurocalc(R, sigma, time, log(S), alpha, N, dpsi);

  const double h_0 = log(K);
  const std::vector<double> logStrikes = fteurocalc.createLogStrikes(h_0);

  std::vector<double> ftCallPrices;
  std::vector<double> bsCallPrices;

  for (unsigned int j = 0; j < computeTill; ++j) {

    const double h = logStrikes[j];
    const double ft_price = fteurocalc.formula_6_CallPrice(h);
    ftCallPrices.push_back(ft_price);

    const Pricing::OptionData optionData(exp(h),
					 maturity,
					 Pricing::OptionData::CALL);
    const double bS_price = pricer.eurocall(optionData, yc, vol, now, S);
    bsCallPrices.push_back(bS_price);
  }

  std::vector<double> fftCallPrices;
  if (fft) 
  fteurocalc.fftCallPrices(h_0, fftCallPrices, &logStrikes[0]);

  for (unsigned int i = 0; i < computeTill; ++i) {
    std::cout << S << ' '
      	      << exp(logStrikes[i]) << ' '
	      << bsCallPrices[i] << ' '
	      << ftCallPrices[i] << ' ';
    if (fft) {
      std::cout << fftCallPrices[i];
    }
    std::cout << std::endl;
  }

  if (fft) {
    const double searchFor = getInput<double>("enter strike: ", true);
  const std::pair<bool, double> interp
    = MathUtils::interpolate(logStrikes, fftCallPrices, log(searchFor));

  if (interp.first) {
    std::cout << searchFor << ' ' << interp.second << std::endl;
  }
  else {
    std::cout << "#error: "
	      << searchFor << " not in range ["
	      << exp(logStrikes[0]) << ", "
	      << exp(logStrikes[N - 1]) << "]"
	      << std::endl;
  }
  }
  return 0;
}

