#include <fstream>

#include "csutils/configfile.h"

void ConfigFile_GUTS::init(const std::string& fileName)
{
  std::ifstream ifs(fileName.c_str());
  while (ifs) {
    std::string key;
    std::string value;
    ifs >> key >> value;
    if (ifs and key[0] != '#') {
      keyValueStore_[key] = value;
    }
  }
}

std::string ConfigFile_GUTS::getValue(const std::string& key) const
{
  std::map<std::string, std::string>::const_iterator
    it = keyValueStore_.find(key);
  if (it != keyValueStore_.end()) {
    return it->second;
  }
  return "";
}

template<> double convert_to_(const std::string& val)
{  return strtod(val.c_str(), 0); }

template<> unsigned int convert_to_(const std::string& val)
{  return strtol(val.c_str(), 0, 10); }

template<> int convert_to_(const std::string& val)
{  return strtol(val.c_str(), 0, 10); }

template<> bool convert_to_(const std::string& val)
{  return (!val.empty()); }

template<> std::string convert_to_(const std::string& val)
{ return val; }
