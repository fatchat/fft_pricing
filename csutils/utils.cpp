#include "csutils/utils.h"

void CMP_nan_check_(CMP z, CMP retVal, const char* name)
{
  if (retVal.real() != retVal.real() or retVal.imag() != retVal.imag()) {
    std::cout << "# found NAN at " << name
	      << " " << z 
	      << " " << retVal
	      << std::endl;
  }
}

