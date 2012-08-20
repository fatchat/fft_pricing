#include <iostream>
#include <boost/smart_ptr.hpp>

#include "csutils/globals.h"
#include "csutils/utils.h"
#include "csutils/configfile.h"
#include "csutils/get_opt.h"
#include "mathutils/accelerators.h"
#include "pricing/asiangammapricer.h"
#include "pricing/agpdata.h"

int main(int argc, char* argv[])
{
  Getopt getopt;
  getopt.addOption("help", Option::NO_ARG);
  getopt.addOption("verbose", Option::NO_ARG);
  getopt.addOption("configfile", Option::REQ_ARG);
  if (getopt.processOpts(argc, argv)) {
    getopt.showHelp(std::cout);
    return 1;
  }
  if (getopt.getOption('h').is_set()) {
    getopt.showHelp(std::cout);
    return 0;
  }
  const bool verbose = getopt.getOption('v').is_set();

  const std::string cfgFile(getopt.getOption('c').is_set() ?
			    getopt.getOption('c').arg() :
			    "configfiles/config.asian");
  ConfigFile::get_mutable_instance().init(cfgFile);

  const double S = get_value_from_config_file("S", 100);
  const double K = get_value_from_config_file("K", 100);
  const double r = get_value_from_config_file("r", 0.09);
  const double sigma = get_value_from_config_file("sigma", 0.05);
  const double time = get_value_from_config_file("time", 1);
  const double A = get_value_from_config_file("A", 22.4);
  const int N = get_value_from_config_file("N", 100);
  
  //typedef ComplexSum< CVZAccelerator<CMP> >     InnerLoopSummer;
  typedef ComplexSum< BruteForce<CMP> >         InnerLoopSummer;
  //typedef RealSum< BruteForce<double> >         InnerLoopSummer;
  //typedef CombinedRealSum<LevinUAccelerator>    InnerLoopSummer;
  //typedef CombinedRealSum< BruteForce<double> > InnerLoopSummer;
  //typedef PosNegSum<LevinUAccelerator>          InnerLoopSummer;
  //typedef RealSum< AitkenAccelerator<double> >  InnerLoopSummer;
  //typedef ComplexSum< EulerAccelerator<CMP> >   InnerLoopSummer;

  boost::shared_ptr<AgpData> agpData(new AgpData(S, r, sigma, time, K, A, N));
  if (verbose) {
    agpData->show_params(&std::cout);
  }

  AsianGammaPricer agp(agpData);
  agp.calcCallPutPrices<InnerLoopSummer>();
  agp.calcFloatingCallPutPrices<InnerLoopSummer>();

  std::cout << "OUTPUT "
	    << agp.asianCallPrice() << " "
	    << agp.asianPutPrice() << " "
	    << agp.asianFloatPutPrice() << ' '
	    << agp.asianFloatCallPrice() << ' '
	    << std::endl;
}
