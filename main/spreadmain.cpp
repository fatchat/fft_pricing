#include <iostream>

#include "csutils/utils.h"
#include "pricing/spreadpricing.h"
#include "csutils/get_opt.h"

int main (int argc, char* argv[])
{
  Getopt getopt;
  getopt.addOption("help", Option::NO_ARG);
  getopt.addOption("show prompts", Option::NO_ARG);
  if (getopt.processOpts(argc, argv)) {
    getopt.showHelp(std::cout);
    return 1;
  }
  if (getopt.getOption('h').is_set()) {
    getopt.showHelp(std::cout);
    return 0;
  }
  bool show = false;
  if (getopt.getOption('s').is_set()) {
    show = true;
  }
  const double S1 = getInput<double>("Share1 Price: ", show);
  const double S2 = getInput<double>("Share2 Price: ", show);
  const double K = getInput<double>("Strike: ", show);
  const double r = getInput<double>("Interest rate: ", show);
  const double sigma1 = getInput<double>("Volatility1: ", show);
  const double sigma2 = getInput<double>("Volatility2: ", show);
  const double rho = getInput<double>("Correlation: ", show);
  const double time = getInput<double>("Time: ", show);
  const double e1 = getInput<double>("Parameter e1: ", show);
  const double e2 = getInput<double>("Parameter e2: ", show);
  const int N = getInput<int>("Number of steps: ", show);

  SpreadPricing sp(S1, S2, K, sigma1, sigma2, rho, r, time);
  const CMP price = sp.spreadpricing(e1, e2, N);
				
  CMP_nan_check_(1, price, "spread price");
  std::cout << "Spread price is: " << price.real() << std::endl;
}
