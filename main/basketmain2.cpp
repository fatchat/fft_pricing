#include <iostream>
#include <boost/numeric/ublas/io.hpp>

#include "csutils/get_opt.h"
#include "csutils/logger.h"
#include "csutils/prog_init.h"
#include "csutils/configfile.h"
#include "mathutils/linalg.h"
#include "pricing/basket.h"

int main(int argc, char* argv[])
{
  Getopt getopt;
  prog_init(getopt, argc, argv);
  const bool verbose = getopt.getOption('v').is_set();

    const std::size_t size = get_value_from_config_file("b", 2);;
    LinAlgSys linalgsys(size);
    Basket::ptr_type basketPtr = create_basket(linalgsys);
    if (verbose) {
      basketPtr->showData(std::cout);
    }

    LinAlgSys::vector_type b_prices;
    LinAlgSys::vector_type s_prices;
    const int N = get_value_from_config_file("N", 100);
      
    try {
      const LinAlgSys::vector_type basket_e = linalgsys.scalar_vector(1.0);
      basketPtr->basketpricingFFT(basket_e, N, b_prices);
    }
    catch (std::exception& e) {
      logthis(std::string("caught exception, exiting: ") + e.what());
      throw;
    }
    try {
      LinAlgSys::vector_type spread_e = linalgsys.make_vector();
      spread_e[0] = -3.0; spread_e[1] = 1.0;
      basketPtr->spreadpricingFFT(spread_e, N, s_prices);
    }
    catch (std::exception& e) {
      logthis(std::string("caught exception, exiting: ") + e.what());
      throw;
    }

    const Basket::stockvec_type& stocks = basketPtr->stockVector();
    assert (stocks.size() == b_prices.size());
    assert (stocks.size() == s_prices.size());

    for(size_t ii = 0; ii < stocks.size(); ++ii) {
      std::cout << stocks[ii] << ' '
		<< "b_val: " << b_prices[ii] << ' '
		<< "s_val: " << s_prices[ii] << ' '
		<< std::endl;
      break;
    }
    return 0;
}



