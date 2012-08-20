#include <algorithm>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "csutils/systemprog.h"
#include "csutils/utils.h"

// =====================================================================
std::vector<std::string> splitter(const std::string& toSplit)
{
  std::vector<std::string> strs;
  boost::split(strs, toSplit, boost::is_any_of(", "));
  return strs;
}

// =====================================================================
class Processor {
public:
  virtual ~Processor() {}
  virtual void process(const std::string&) const = 0;
};

// =====================================================================
class ProgramTester : public Processor {
  const std::string progName_;
  std::ostream& os_;
public:
  ProgramTester(const std::string& progName, std::ostream& os)
    : progName_(progName)
    , os_(os)
  { }
  virtual void process(const std::string& cfgStr) const
  {
    const std::string cfgfilename("configfiles/__temp__findparams__");
    {
      std::ofstream ofs(cfgfilename.c_str());
      ofs << cfgStr;
    }
    const std::string runCmd(progName_ + " -c " + cfgfilename);
    SystemProg systemProg(runCmd);
    while (!systemProg.done()) {
      const std::string input(systemProg.getLine(/*chop=*/true));
      if (input.length()) {
	os_ << input << std::endl;
      }
    }
  }
};

// =====================================================================
class ParamBase {
public:
  typedef boost::shared_ptr<ParamBase>        param_ptr;
  typedef std::vector<param_ptr>               param_ptr_vec;
  typedef const std::vector<param_ptr>&        param_ptr_vec_cref;
  typedef const std::string&                  str_cref;
  typedef boost::shared_ptr<const Processor>  cproc_ptr;

  virtual ~ParamBase() { }
  virtual void iterate(void (*gen)(param_ptr_vec_cref, unsigned int,
				   str_cref, cproc_ptr),
		       param_ptr_vec_cref, unsigned int, str_cref,
		       cproc_ptr) const = 0;
};

// =====================================================================
template<class T>
T fromString(const std::string&);
template<>
int fromString(const std::string& s) { return atoi(s.c_str()); }
template<>
double fromString(const std::string& s) { return atof(s.c_str()); }

// =====================================================================
template<class T>
class Param : public ParamBase {
  const std::string name_;
  const T start_val_;
  const T end_val_;
  const T step_;
public:
  Param(const std::vector<std::string>& params)
    : name_(params.at(0))
    , start_val_(fromString<T>(params.at(1)))
    , end_val_(fromString<T>(params.at(2)))
    , step_(fromString<T>(params.at(3)))
  { }

  virtual void iterate(void (*gen)(param_ptr_vec_cref, unsigned int,
				   str_cref, cproc_ptr),
		       param_ptr_vec_cref params, unsigned int index,
		       str_cref soFar, cproc_ptr processor) const
  {
    for (T val = start_val_; val != end_val_; val += step_) {
      const std::string outStr = soFar + name_ + " " + toString(val) + "\n";
      (*gen)(params, index + 1, outStr, processor);
    }
  }
};

// =====================================================================
void generate(ParamBase::param_ptr_vec_cref params, unsigned int index,
	      ParamBase::str_cref soFar, ParamBase::cproc_ptr processor)
{
  assert(processor);
  if (params.size() == index) {
    processor->process(soFar);  
    return;
  }
  params[index]->iterate(&generate, params, index, soFar, processor);
}

// =====================================================================
ParamBase::param_ptr_vec get_params(const std::string& configFile)
{
  ParamBase::param_ptr_vec params;
  std::ifstream ifs(configFile.c_str());
  std::string line;
  while (std::getline(ifs, line)) {
    if (line[0] == '#' or line.empty()) {
      continue;
    }
    std::vector<std::string> strs = splitter(line);
    if (strs.size() != 5) {
      throw std::length_error(std::string("Error at line: ") + line);
    }
    const std::string type = strs[0];
    strs.erase(strs.begin());
    ParamBase* new_param = 0;
    if (type == "double") {
      new_param = new Param<double>(strs);
    }
    else if (type == "int") {
      new_param = new Param<int>(strs);
    }
    else {
      throw std::range_error(std::string("Unknown type: ") + type);
    }
    assert(new_param);
    params.push_back(ParamBase::param_ptr(new_param));
  }
  return params;
}

// =====================================================================
void showHelp(const std::string& progName)
{
  std::cout << "usage: "
	    << progName << ' '
	    << "<program to test> <config file>\n"
    ;
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    showHelp(argv[0]);
    return 1;
  }
  if (std::string(argv[1]) == "-h" or std::string(argv[1]) == "--help") {
    showHelp(argv[0]);
    return 0;
  }
  const std::string progName(argv[1]);
  const std::string configFile(argv[2]);

  ParamBase::param_ptr_vec params(get_params(configFile));
  ParamBase::cproc_ptr pt(new ProgramTester(progName, std::cout));
  generate(params, 0, "", pt);

  return 0;
}

