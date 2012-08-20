#include <iostream>
#include <cmath>

#include "csutils/utils.h"
#include "csutils/globals.h"
#include "csutils/get_opt.h"
#include "mathutils/gauss_legendre.h"


class Integrate_x {
  int exp_;
public:
  Integrate_x(int exponent)
    : exp_(exponent) {}
  double f(double x) const { return pow(x, exp_); }
};

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

    const Integrate_x int_x(1);
    GaussLegendre<Integrate_x> gl(int_x, &Integrate_x::f);
    const double result = gl.integrate(2, 0, 1);
    std::cout << "integral is " << result << std::endl;

    return 0;
}
