#ifndef SYSTEMPROG_H
#define SYSTEMPROG_H

#include <string>
#include <cstdio>

class SystemProg {
  const std::string progName_;
  FILE* fp_;
  char data_[80];
 public:
  SystemProg(const std::string& progName, bool dieOnFail = false);
  ~SystemProg();
  bool done() const { return fp_ == 0; }
  std::string getLine(bool chop = false);
};

#endif // SYSTEMPROG_H
