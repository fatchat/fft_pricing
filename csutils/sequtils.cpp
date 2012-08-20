#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>
#include <cassert>

#include "csutils/sequtils.h"

namespace SeqUtils {

  void split_positive_negative(std::vector<double>& source,
			       std::vector<double>& positive,
			       std::vector<double>& negative)
  {
    std::sort(source.begin(), source.end(), std::less<double>());
    std::vector<double>::iterator middle = std::lower_bound(source.begin(),
							    source.end(),
							    0.0);
    assert(middle == source.end() or *middle >= 0.0);
    positive.clear();
    negative.clear();
    // 0.0 goes to positive if it exists in source
    std::copy(source.begin(), middle, std::back_inserter(negative));
    std::copy(middle, source.end(), std::back_inserter(positive));
    assert(positive.size() + negative.size() == source.size());
  }
  
  void combine_positive_negative(const std::vector<double>& pos,
				 const std::vector<double>& neg,
				 std::vector<double>& combined)
  {
    combined.clear();
    const unsigned int sz = std::min(pos.size(), neg.size());
    unsigned int ii = 0;
    for (; ii < sz; ++ii) {
      combined.push_back(pos.begin()[ii] - neg.begin()[ii]);
    }
    if (ii < pos.size()) {
      std::copy(pos.begin() + ii, pos.end(), std::back_inserter(combined));
    }
    else if (ii < neg.size()) {
      std::copy(neg.begin() + ii, neg.end(), std::back_inserter(combined));
    }
    else {
      ;
      assert(pos.size() == neg.size());
    }
  }

  void real_part(std::vector<CMP> cmpSeries, std::vector<double>& realSeries)
  {
    realSeries.resize(cmpSeries.size());
    std::transform(cmpSeries.begin(), cmpSeries.end(), realSeries.begin(),
		   std::mem_fun_ref<double&, CMP>(&CMP::real));
  }

  void imag_part(std::vector<CMP> cmpSeries, std::vector<double>& realSeries)
  {
    realSeries.resize(cmpSeries.size());
    std::transform(cmpSeries.begin(), cmpSeries.end(), realSeries.begin(),
		   std::mem_fun_ref<double&, CMP>(&CMP::imag));
  }

  void split_real_imag(const std::vector<CMP>& cmpSeries,
		       std::vector<double>& realSeries,
		       std::vector<double>& imagSeries)
  {
    realSeries.clear(); 
    imagSeries.clear();
  
    typedef std::vector<CMP>::const_iterator c_iter;

    const c_iter seq_end = cmpSeries.end();
    for(c_iter ii = cmpSeries.begin(); ii != seq_end; ++ii) {
      realSeries.push_back(ii->real());
      imagSeries.push_back(ii->imag());
    }
  }

  void prep_real_series(std::vector<double>& series, double upper_bound)
  {
    std::sort(series.begin(), series.end(), std::greater<double>());
    if (upper_bound > 0.0) {
      const std::vector<double>::iterator limit
	= std::lower_bound(series.begin(), series.end(),
			   upper_bound, std::greater<double>());
      series.erase(limit, series.end());
    }
  }

  void prep_complex_series(std::vector<CMP>& series, double upper_bound)
  {
    std::sort(series.begin(), series.end(), CMP_ABS_GREATER());
    if (upper_bound > 0.0) {
      const std::vector<CMP>::iterator limit
	= std::lower_bound(series.begin(), series.end(),
			   upper_bound, CMP_ABS_GREATER());
      series.erase(limit, series.end());
    }
  }

} // namespace
