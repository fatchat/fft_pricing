#include <stdexcept>

#include "csutils/systemprog.h"

SystemProg::SystemProg(const std::string& progName, bool dieOnFail)
  : progName_(progName)
  , fp_(0) 
{
  fp_ = popen(progName_.c_str(), "r");
  if (!fp_ and dieOnFail) {
    throw std::runtime_error(std::string("Could not run ") + progName);
  }
}

SystemProg::~SystemProg()
{
  if (fp_) {
    pclose(fp_);
    fp_ = 0;
  }
}

std::string SystemProg::getLine(bool chop)
{
  if (fp_) {
    if(fgets(data_, sizeof(data_), fp_)) {
      std::string retVal(data_);
      if (chop) {
	retVal.erase(retVal.find_first_of("\n"), 1);
      }
      return retVal;
    }
    else {
      pclose(fp_);
      fp_ = 0;
    }
  }
  return "";
}

