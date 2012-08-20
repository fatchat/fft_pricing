#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <vector>

#include "csutils/globals.h"

template<class returnType>
returnType getInput(const std::string& prompt, bool echo = false)
{
  if (echo) { std::cerr << prompt; }
  returnType input;  std::cin >> input;  return input;
}

template<class T>
std::string toString(T data)
{  std::stringstream s; s << data; return s.str(); }

// use with remove_if
template<class T>
bool deletePtr(T* p) { if (p) delete p; p = 0; return true; }

template<class T>
void
show_vector(const std::vector<T>& v, const std::string& tag, std::ostream* os)
{ if (!os) return;
  const unsigned int sz = v.size();
  for (unsigned int ii = 0; ii < sz; ++ii)
   { *os << tag << ' ' << ii << ' ' << v[ii] << std::endl; }
}

void CMP_nan_check_(CMP z, CMP retVal, const char* name);

template<class T>
void check_nz(T arg, const std::string& varname)
{ if (arg == T(0))
  { throw std::invalid_argument(varname + " must be nonzero"); }
}

struct ABS_LESS : public std::binary_function<double, double, bool>
{ bool operator()(const double& a, const double& b) const
  { return (fabs(a) < fabs(b)); } };

struct ABS_GREATER : public std::binary_function<double, double, bool>
{ bool operator()(const double& a, const double& b) const
  { return (fabs(a) > fabs(b)); } };

struct CMP_ABS_LESS : public std::binary_function<CMP, CMP, bool>
{ bool operator()(const CMP& a, const CMP& b) const
  { return (abs(a) < abs(b)); } };

struct CMP_ABS_GREATER : public std::binary_function<CMP, CMP, bool>
{ bool operator()(const CMP& a, const CMP& b) const
  { return (abs(a) > abs(b)); } };

#endif //UTILS_H
