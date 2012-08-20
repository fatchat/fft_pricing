#include <iostream>
#include <fstream>
#include <string>

#include "csutils/globals.h"
#include "csutils/utils.h"
#include "csutils/configfile.h"
#include "csutils/get_opt.h"
#include "csutils/logger.h"
#include "csutils/prog_init.h"
#include "pricing/basketpricing.h"

int main(int argc, char* argv[])
{
  Getopt getopt;
  prog_init(getopt, argc, argv);
  const bool verbose = getopt.getOption('v').is_set();

  //const double e1 = get_value_from_config_file("e_0", -2.1);
  //const double e2 = get_value_from_config_file("e_1", 1.);
  const int N = get_value_from_config_file("N", 100);
  const double delta = get_value_from_config_file("delta", 0.25);

  BasketPricing bp(create_basketdata(), delta);
  if (verbose) {
    std::ostringstream ostr;
    bp.basketData().show_params(&ostr);
    ostr// << " e1: " << e1
	// << " e2: " << e2
	 << " N: " << N
      ;
    std::cout << ostr.str() << std::endl;
    logthis(ostr.str());
    ostr.str("");
  }

  const CMP b_val = bp.basketpricing(1.0, 1.0, N);
  const CMP s_val = bp.spreadpricing(-3.0, 1.0, N);
  LOG("b_val: " << b_val << " s_val: " << s_val);
  std::cout << "b_val: " << b_val << " s_val: " << s_val << std::endl;
}
