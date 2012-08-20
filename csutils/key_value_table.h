#ifndef KEY_VALUE_TABLE_H
#define KEY_VALUE_TABLE_H

#include <vector>
#include <string>
#include <iostream>

// this is not a map, which could just be implemented using std::map

template<class KeyType, class ValueType>
class KeyValueTable {

  std::vector<KeyType> keys_;
  std::vector<ValueType> values_;

  void read_from_stream_(std::istream&);

  KeyValueTable();
  KeyValueTable(const KeyValueTable&);
  KeyValueTable& operator=(const KeyValueTable&);

 public:
  KeyValueTable(const std::string& filename);
  KeyValueTable(std::istream& instream);

  const std::vector<KeyType>& keys() const { return keys_; }
  const std::vector<ValueType>& values() const { return values_; }
};

class InterpolatedKVT {

  const KeyValueTable<double, double> kvt_;

  InterpolatedKVT();
  InterpolatedKVT(const InterpolatedKVT&);
  InterpolatedKVT& operator=(const InterpolatedKVT&);

 public:

  InterpolatedKVT(const std::string& filename)
    : kvt_(filename)
  { }
  InterpolatedKVT(std::istream& is)
    : kvt_(is)
  { }

  const KeyValueTable<double, double>& kvt() const { return kvt_; }
  bool getVal(double key, double& retVal) const;

  // no checking
  double operator[](double) const;
};

#include "csutils/key_value_table.hpp"

#endif // KEY_VALUE_TABLE_H
