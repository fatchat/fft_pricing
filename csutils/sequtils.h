#ifndef SEQUENCEUTILS_H
#define SEQUENCEUTILS_H

#include "csutils/globals.h"
#include "csutils/utils.h"

namespace SeqUtils {

  void split_positive_negative(std::vector<double>& source,
			       std::vector<double>& positive,
			       std::vector<double>& negative);

  void combine_positive_negative(const std::vector<double>& pos,
				 const std::vector<double>& neg,
				 std::vector<double>& combined);

  void split_real_imag(const std::vector<CMP>& cmpSeries,
		       std::vector<double>& realSeries,
		       std::vector<double>& imagSeries);

  // note: complex series passed by value (cannot pass by const-ref)
  void real_part(std::vector<CMP>, std::vector<double>&);
  void imag_part(std::vector<CMP>, std::vector<double>&);

  void prep_real_series(std::vector<double>& series, double upper_bound);
  void prep_complex_series(std::vector<CMP>& series, double upper_bound);

} // namespace

#endif // SEQUENCEUTILS_H
