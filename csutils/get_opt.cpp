#include "csutils/get_opt.h"
#include <vector>
#include <iostream>

Option::Option(const char* long_name, arg_type arg)
  : shortOpt_(long_name[0])
  , is_set_(false)
  , long_name_(long_name)
{
  longOpt_.name = long_name_.c_str();
  longOpt_.has_arg = (int)arg;
  longOpt_.flag = 0;
  longOpt_.val = shortOpt_;
}

void Option::make_usage()
{
  usage_ = " [-";
  usage_ += shortOpt_;
  if (longOpt_.has_arg == int(NO_ARG)) {
    usage_ += '(';
    usage_ += std::string(longOpt_.name).substr(1);
    usage_ += ')';
  }
  else { // if (arg == REQ_ARG) {
    usage_ += " <";
    usage_ += longOpt_.name;
    usage_ += '>';
  }
  usage_ += "]";
}

void Getopt::addOption(const char* long_name, Option::arg_type arg)
{
  const char short_opt = long_name[0];
  if (short_opt) {
    options_[short_opt] = Option(long_name, arg);
  }
}

int Getopt::processOpts(int argc, char* argv[])
{
  usage_ = "usage: ";
  usage_ += argv[0];
  std::string shortString;
  for(std::map<char, Option>::const_iterator i = options_.begin();
      i != options_.end(); ++i)
  {
    const Option& opt = i->second;
    shortString += opt.shortOpt();
    if (opt.hasArg()) {
      shortString += ':';
    }
  }
  std::vector<option> options_vector;
  for(std::map<char, Option>::iterator i = options_.begin();
      i != options_.end(); ++i) {
    options_vector.push_back(i->second.longOpt());
  }
  while (1) {
    int indexPtr;
    const int ret
      = getopt_long(argc, argv,
		    shortString.c_str(),
		    &options_vector[0],
		    &indexPtr);
    if (ret < 0) {
      break;
    }
    if (ret == int('?')) {
      //std::cout << "ERROR: encountered an unknown option\n";
      return 1;
    }
    //std::cout << "encountered " << (char)ret << std::endl;
    Option& the_option = options_[(char)ret];
    the_option.SET();
    if (the_option.hasArg()) {
      //std::cout << "   argument=" << optarg << std::endl;
      the_option.SET_ARG(optarg);
    }
  }
  return 0;
}

void Getopt::showHelp(std::ostream& os)
{
  for(std::map<char, Option>::iterator i = options_.begin();
      i != options_.end(); ++i)
  {
    Option& opt = i->second;
    opt.make_usage();
    usage_ += opt.usage_;
  }
  usage_ += "\n";
  os << usage_;
}
