#include <iostream>

#include "csutils/utils.h"
#include "csutils/globals.h"
#include "csutils/get_opt.h"
#include "mathutils/euler_acc.h"


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

  std::vector<CMP> a;
#if 0
  while (std::cin) {
    double inp;
    std::cin >> inp;
    if (std::cin and inp != 0. ) {
      a.push_back(CMP(inp, 0));
    }
  }
#else
  for (unsigned int ii = 0; ii < 50; ++ii) {
    a.push_back(pow(c_I, ii));
  }
#endif
  EulerAccelerator<CMP> acc(a);
  std::cout << acc.sum() << std::endl;
  return 0;
}
