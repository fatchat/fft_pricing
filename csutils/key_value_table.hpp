#include <fstream>

template<class KeyType, class ValueType>
  KeyValueTable<KeyType, ValueType>::KeyValueTable(const std::string& filename)
{
  std::ifstream ifs(filename.c_str());
  if (ifs) {
    read_from_stream_(ifs);
  }
}

template<class KeyType, class ValueType>
  KeyValueTable<KeyType, ValueType>::KeyValueTable(std::istream& is)
{
  read_from_stream_(is);
}

template<class KeyType, class ValueType>
void  KeyValueTable<KeyType, ValueType>::read_from_stream_(std::istream& is)
{
  while (is) {
    KeyType key;
    ValueType value;
    is >> key >> value;
    if (is) {
      //std::cout << "KEY:[" << key << "], VALUE:[" << value << "]\n";
      keys_.push_back(key);
      values_.push_back(value);
    }
  }
}
