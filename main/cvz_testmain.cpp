#include <iostream>

#include "csutils/utils.h"
#include "csutils/globals.h"
#include "csutils/get_opt.h"
#include "mathutils/cvz_accel.h"


int main(int argc, char* argv[])
{
    Getopt getopt;
    getopt.addOption("help", Option::NO_ARG);

    if (getopt.processOpts(argc, argv)) {
      getopt.showHelp(std::cout);
        return 1;
    }
    if (getopt.getOption('h').is_set()) {
      getopt.showHelp(std::cout);
        return 0;
    }

    std::vector<double> summands;
    while (std::cin) {
      double inp;
      std::cin >> inp;
      if (std::cin and inp != 0. ) {
	summands.push_back(inp);
      }
    }

    CVZAccelerator<double> acc(summands);
    std::cout << "sum=" << acc.sum() / (1-sqrt(2)) << std::endl;

    return 0; 
}
