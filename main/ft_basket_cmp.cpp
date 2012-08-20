#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#include <boost/format.hpp>

#include "csutils/get_opt.h"
#include "csutils/logger.h"
#include "csutils/prog_init.h"
#include "csutils/configfile.h"
#include "mathutils/linalg.h"
#include "pricing/fteurocalc.h"
#include "pricing/basket.h"


int main(int argc, char* argv[])
{
  Getopt getopt;
  prog_init(getopt, argc, argv);

  const bool verbose = getopt.getOption('v').is_set();
  
  const std::size_t size = 1;
  LinAlgSys linalgsys(size);
  Basket::ptr_type basketPtr = create_basket(linalgsys);

  if (verbose) {
    basketPtr->showData(std::cout);
  }

  const int N = get_value_from_config_file("N", 100);
  assert(N > 0);
  LinAlgSys::vector_type prices;

  const LinAlgSys::vector_type e = linalgsys.scalar_vector(1.0);
  LOG("e1: " << e[0] <<  " N: " << N);
  if (verbose) {
    std::cout << "e1: " << e[0] << " N: " << N << std::endl;
  }
  basketPtr->basketpricingFFT(e, N, prices);

  FTEuroCalc fteurocalc(get_value_from_config_file("r", 0.09),
			get_value_from_config_file("sigma_0", 0.05),
			get_value_from_config_file("time", 1.),	
			log(get_value_from_config_file("S_0", 100.)),
			get_value_from_config_file("alpha", 1.),
			N,
			get_value_from_config_file("delta", 0.25));
  
  const double K = get_value_from_config_file("K", 100.);
  const double ft_price = fteurocalc.formula_6_CallPrice(log(K));
  
  std::vector<double> fftCallPrices;
  fteurocalc.fftCallPrices(log(K), fftCallPrices, 0);
  const double fft_price = fftCallPrices[0];
  
  std::cout << boost::format("%d %3.8d %3.8d %3.8d\n") % K % prices[0] % ft_price % fft_price;

  return 0;
}

