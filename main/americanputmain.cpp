#include <iostream>

#include "csutils/utils.h"
#include "csutils/globals.h"
#include "csutils/get_opt.h"
#include "pricing/americanput.h"


int main(int argc, char* argv[])
{
    Getopt getopt;
    getopt.addOption("help", Option::NO_ARG);
    getopt.addOption("show_prompts", Option::NO_ARG);
    getopt.addOption("graph", Option::NO_ARG);
    if (getopt.processOpts(argc, argv)) {
      getopt.showHelp(std::cout);
        return 1;
    }
    if (getopt.getOption('h').is_set()) {
      getopt.showHelp(std::cout);
        return 0;
    }
    const bool show = getopt.getOption('s').is_set();
    const bool graph = getopt.getOption('g').is_set();

    const double S = getInput<double>("Share price: ", show);
    const double K = getInput<double>("Strike: ", show);
    const double R = getInput<double>("Interest rate: ", show);
    const double sigma = getInput<double>("Volatility: ", show);
    const double time = getInput<double>("Time: ", show);
    const unsigned int N = getInput<int>("Number of steps: ", show);

    std::cout << "# using params "
	      << "S=" << S
	      << " K=" << K
	      << " R=" << R
	      << " sigma=" << sigma
	      << " time=" << time
	      << " N=" << N
	      << std::endl;

    AmericanPut americanPut(S, K, R, sigma, time, N);
    americanPut.show_cached_vals(std::cout);
    if (graph) {
      americanPut.graph(std::cout);
      return 0;
    }

    const double pp = americanPut.perpetual_price();
    const double ftp = americanPut.finite_time_price();
    const double pe = americanPut.perpetual_exercise();
    const double oe = americanPut.optimal_exercise();

    std::cout << "perpetual price=" << pp << ' '
	      << "finite time price=" << ftp << ' '
	      << "perpetual exercise=" << pe << ' '
	      << "optimal exercise=" << oe << ' '
	      << std::endl;

    return 0;
}
