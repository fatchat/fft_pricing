#include <iostream>
#include <vector>
#include <iterator>

#include "csutils/sequtils.h"
#include "csutils/globals.h"

template<class T>
void print(const std::vector<T>& seq)
{
  std::copy(seq.begin(), seq.end(), std::ostream_iterator<T>(std::cout, "\n"));
}

int main()
{
  std::vector<double> seq;
  for (int ii = 0; ii < 20; ++ii) {
    seq.push_back(ii * 1.0);
  }
  SeqUtils::prep_real_series(seq, 5.0);
  print(seq);

  std::vector<CMP> cseq;
  for (int ii = 0; ii < 20; ++ii) {
    cseq.push_back(CMP(ii * 1.0, 0.0));
  }
  SeqUtils::prep_complex_series(cseq, 5.0);
  print(cseq);
}
