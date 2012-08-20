#include <iostream>
#include <cstdio>

#include "csutils/utils.h"
#include "csutils/globals.h"
#include "csutils/get_opt.h"
#include "csutils/configfile.h"
#include "pricing/asiangammapricer.h"
#include "mathutils/interpolate.h"
#include "mathutils/accelerators.h"

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
    const double sigma = get_value_from_config_file("sigma", 0.1);
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
    const std::vector<double> logStrikes = agpData->create_log_strikes();  
    assert(logStrikes[0] == agpData->scale_strike(K));
    const unsigned int nStrikes = logStrikes.size();

    AsianGammaPricer agp(agpData);
    const std::vector<CMP> fftVals
      = agp.fastasianpricingFFT_<InnerLoopSummer>(&logStrikes);

    std::vector<double> allStrikes;
    for (unsigned int i = 0; i < nStrikes; ++i) {
      allStrikes.push_back(agpData->unscale_strike(logStrikes[i]));
      //std::cout << i << " " << aS << std::endl;
    }

#if 0  // test fft
    for (unsigned int i = 0; i < nStrikes; ++i) {
      const double KK = allStrikes[i];
      if (KK - S > 20) { break; }
      boost::shared_ptr<AgpData>
	agpData_i(new AgpData(S, r, sigma, time, KK, A, N));
      if (verbose) {
	agpData_i->show_params(&std::cout);
      }
      agp.newParams(agpData_i, true);
      const CMP FAP_i = agp.fastasianpricing_<InnerLoopSummer>();
      printf("[%d] %7.4f <- should equal -> %7.4f\n", i, FAP_i, fftVals[i]);
    }
    return 0;
#endif

    std::vector<double> realFFTVals;
    for (unsigned j = 0; j < fftVals.size(); ++j) {
      realFFTVals.push_back(fftVals[j].real());
    }
    for (double searchFor = agp.agpData()->S_ - 1.0;
	 searchFor < agp.agpData()->S_ + 20.0;
	 searchFor += 0.50) {
      const std::pair<bool, double> interp
	= MathUtils::interpolate(allStrikes, realFFTVals, searchFor);
      if (interp.first) {
	printf("%3.2f %5.4f\n", searchFor, interp.second);
      }
      else {
	printf("#error: %7.4f not in range [%7.4f, %7.4f]\n",
	       searchFor, allStrikes[0], allStrikes[N-1]);
      }
    }
    
    return 0;
}
