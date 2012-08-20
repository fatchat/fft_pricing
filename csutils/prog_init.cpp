#include <iostream>
#include <cstdlib>

#include "csutils/logger.h"
#include "csutils/configfile.h"

#include "csutils/prog_init.h"

std::string get_prog_name(const std::string& full_path)
{
  std::string::size_type pos = full_path.find_last_of('/');
  if (pos == std::string::npos) {
    return full_path;
  }
  std::string::size_type pos2 = full_path.substr(pos).find_first_not_of('/');
  assert (pos2 != std::string::npos);
  return full_path.substr(pos).substr(pos2);
}

void prog_init(Getopt& getopt, int argc, char* argv[])
{
  getopt.addOption("help", Option::NO_ARG);
  getopt.addOption("verbose", Option::NO_ARG);
  getopt.addOption("configfile", Option::REQ_ARG);
  getopt.addOption("logfile", Option::REQ_ARG);
  if (getopt.processOpts(argc, argv)) {
    getopt.showHelp(std::cout);
    exit(1);
  }
  if (getopt.getOption('h').is_set()) {
    getopt.showHelp(std::cout);
    exit(0);
  }
  
  const bool verbose = getopt.getOption('v').is_set();
  std::string default_cfg = "configfiles/config.";
  std::string default_log = "logfiles/";
  std::string prog_name = get_prog_name(argv[0]);
  default_cfg += prog_name;
  default_log += prog_name + ".log";
  //  std::cout << default_cfg << ' ' << default_log << std::endl;
  const std::string cfgFile(getopt.getOption('c').is_set() ?
			    getopt.getOption('c').arg() :
			    default_cfg);
  ConfigFile::get_mutable_instance().init(cfgFile);
  if (verbose) {
    std::cout << "using config file " << cfgFile << std::endl;
  }

  const std::string logFile(getopt.getOption('l').is_set() ?
			    getopt.getOption('l').arg() :
			    default_log);
  Logger::init(logFile);
  if (verbose) {
    std::cout << "writing to log file " << logFile << std::endl;
  }
}
