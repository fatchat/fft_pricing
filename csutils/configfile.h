#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <map>
#include <string>
#include <cstdlib>

#include "boost/serialization/singleton.hpp"

class ConfigFile_GUTS {
  std::map<std::string, std::string> keyValueStore_;
 public:
  void init(const std::string& fileName);
  std::string getValue(const std::string& key) const;
};

typedef boost::serialization::singleton<ConfigFile_GUTS> ConfigFile;

template<class T> T convert_to_(const std::string&);

template <class T>
T get_value_from_config_file(const std::string& key, T defaultVal)
{
  const std::string val = ConfigFile::get_const_instance().getValue(key);
  const T retVal = val == "" ? defaultVal : convert_to_<T>(val);
  return retVal;
}

#endif // CONFIGFILE_H
