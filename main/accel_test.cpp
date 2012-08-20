#include <iostream>
#include <iomanip>
#include <vector>

#include "csutils/utils.h"
#include "csutils/globals.h"
#include "csutils/get_opt.h"
#include "mathutils/gamma.h"
#include "mathutils/levinu_acc.h"


void createGammaValues(std::ostream& os, unsigned int SIZE)
{
  std::vector<double> terms(SIZE);
  for (unsigned int i = 0; i < SIZE; ++i) {
    terms[i] = Gamma::log_gsl_gamma(1 + 1.0/ (2 * (i + 1) - 1)).real()
      - Gamma::log_gsl_gamma(1 + 1.0 / (2 * (i + 1))).real();
  }
  for (unsigned int ii = 0; ii < terms.size(); ++ii) {
    os << std::setprecision(20) << terms[ii] << std::endl;
  }
}

void log2(std::ostream& os, unsigned int SIZE)
{
  for (unsigned int i = 0; i < SIZE; ++i) {
    os << std::setprecision(20) << 1.0 / (i + 1) << std::endl;
  }
}

void zeta_half(std::ostream& os, unsigned int SIZE)
{
  for (unsigned int i = 0; i < SIZE; ++i) {
    os << std::setprecision(20) << 1.0 / pow(i + 1, 0.5) << std::endl;
  }
}

int main(int argc, char* argv[])
{
    Getopt getopt;
    getopt.addOption("help", Option::NO_ARG);
    getopt.addOption("gamma", Option::NO_ARG);
    getopt.addOption("log2", Option::NO_ARG);
    getopt.addOption("zetahalf", Option::NO_ARG);
    getopt.addOption("terms", Option::REQ_ARG);

    if (getopt.processOpts(argc, argv)) {
      getopt.showHelp(std::cout);
	  return 1;
    }

    if (getopt.getOption('h').is_set()) {
      getopt.showHelp(std::cout);
	  return 0;
    }

    unsigned int SIZE = 12000;
    if (getopt.getOption('t').is_set()) {
      SIZE = atoi(getopt.getOption('t').arg().c_str());
    }

    if (getopt.getOption('g').is_set()) {
      createGammaValues(std::cout, SIZE);
      return 0;
    }

    if (getopt.getOption('l').is_set()) {
      log2(std::cout, SIZE);
      return 0;
    }

    if (getopt.getOption('z').is_set()) {
      zeta_half(std::cout, SIZE);
      return 0;
    }

    // default behaviour
    std::vector<double> a;
    while (std::cin) {
      double inp;
      std::cin >> inp;
      if (std::cin and inp != 0. ) {
		a.push_back(inp);
      }
    }

    std::cout << "size=" << a.size() << std::endl;
    unsigned int accsize = a.size();
    if (getopt.getOption('t').is_set()) {
      accsize = atoi(getopt.getOption('t').arg().c_str());
    }

    LevinUAccelerator acc(a);
    const double sum = acc.sum();
    std::cout << "sum=" << sum << std::endl;
    std::cout << "exp= " << exp(sum) << std::endl;

    return 0;
}
