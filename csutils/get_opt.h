#ifndef GET_OPT_H
#define GET_OPT_H

#include <string>
#include <map>
#include <iosfwd>

#include <getopt.h>

extern int opterr;
extern int optind;
class Getopt;
class Option {
  friend class Getopt;
  option longOpt_;
  char shortOpt_;
  bool is_set_;
  std::string long_name_;
  std::string arg_;
  std::string usage_;
public:
  enum arg_type { 
    NO_ARG = no_argument,
    REQ_ARG = required_argument
  };
  Option() : shortOpt_('?'), is_set_(false), long_name_("") {}
private:
  Option(const char* long_name, arg_type arg);
  void SET() { is_set_ = true; }
  void SET_ARG(const std::string& arg) { arg_ = arg; }
public:
  const option& longOpt() const { return longOpt_; }
  char shortOpt() const { return shortOpt_; }
  const std::string& arg() const { return arg_; }
  bool is_set() const { return is_set_; }
  bool hasArg() const { return (longOpt_.has_arg == required_argument); }
  void make_usage();
};

class Getopt
{
  std::map<char, Option> options_;
  std::string usage_;
public:
  Getopt(bool showErrorMessage = false) { opterr = (int)showErrorMessage; }
  void addOption(const char* long_name, Option::arg_type);
  const Option& getOption(char c) { return options_[c]; }
  int processOpts(int argc, char* argv[]);
  int first_non_opt() const { return optind; }
  void showHelp(std::ostream&);
};

#endif // GET_OPT_H
