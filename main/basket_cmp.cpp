#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#include <boost/format.hpp>

#include "csutils/get_opt.h"
#include "csutils/logger.h"
#include "csutils/prog_init.h"
#include "csutils/configfile.h"
#include "mathutils/linalg.h"
#include "pricing/basket.h"
#include "pricing/basketpricing.h"

int main(int argc, char* argv[])
{
  Getopt getopt;
  prog_init(getopt, argc, argv);
  const bool verbose = getopt.getOption('v').is_set();
  
  const std::size_t size = 2;
  LinAlgSys linalgsys(size);
  Basket::ptr_type basketPtr = create_basket(linalgsys);
  const double delta = get_value_from_config_file("delta", 0.25);
  BasketPricing bp(create_basketdata(), delta);
  if (verbose) {
    basketPtr->showData(std::cout);
    bp.basketData().show_params(&std::cout);
  }

  LinAlgSys::vector_type prices;
  CMP price;
  try {
    const LinAlgSys::vector_type e = linalgsys.scalar_vector(1.0);
    const int N = get_value_from_config_file("N", 100);
    LOG("e1: " << e[0] << " e2: " << e[1] << " N: " << N);
    if (verbose) {
      std::cout << "e1: " << e[0] << " e2: " << e[1]
		<< " N: " << N<< std::endl;
    }
    basketPtr->basketpricingFFT(e, N, prices);
    price = bp.basketpricing(e[0], e[1], N);
  }
  catch (std::exception& e) {
    logthis(std::string("caught exception, exiting: ") + e.what());
    throw;
  }

  assert(bp.basketData().S1_ == basketPtr->stockVector()[0][0]);
  assert(bp.basketData().S2_ == basketPtr->stockVector()[0][1]);
  std::cout << boost::format("%3.8d %3.8d %3.8d\n") 
    % prices[0] % price.real() % (prices[0] - price.real());
  return 0;
}

