#include "csutils/sequtils.h"

template<class Accelerator>
ComplexSum<Accelerator>::ComplexSum(const std::vector<CMP>& _series,
				    double bound)
  : genericSum_(_series)
{
  SeqUtils::prep_complex_series(genericSum_.seriesRef(), bound);
}

template<class Accelerator>
CMP ComplexSum<Accelerator>::sum()
{
  return Accelerator(genericSum_.series()).sum();
}

template<class Accelerator>
RealSum<Accelerator>::RealSum(const std::vector<CMP>& _series,
			      double bound)
  : genericSum_(_series)
{
  SeqUtils::split_real_imag(genericSum_.series(), real_series_, imag_series_);
  SeqUtils::prep_real_series(real_series_, bound);
  SeqUtils::prep_real_series(imag_series_, bound);
}

template<class Accelerator>
CMP RealSum<Accelerator>::sum()
{
  const double realSum = Accelerator(real_series_).sum();
  const double imagSum = Accelerator(imag_series_).sum();
  return CMP(realSum, imagSum);
}

template<class Accelerator>
PosNegSum<Accelerator>::PosNegSum(const std::vector<CMP>& _series, double bound)
  : RealSum<Accelerator>(_series, bound)
  , real_(RealSum<Accelerator>::real_series())
  , imag_(RealSum<Accelerator>::imag_series())
{ }

template<class Accelerator>
CMP PosNegSum<Accelerator>::sum()
{
  const double rpsum = Accelerator(real_.pos()).sum();
  const double rnsum = Accelerator(real_.neg()).sum();
  const double ipsum = Accelerator(imag_.pos()).sum();
  const double insum = Accelerator(imag_.neg()).sum();
  return CMP(rpsum - rnsum, ipsum - insum);
}

template<class Accelerator>
CombinedRealSum<Accelerator>::CombinedRealSum(const std::vector<CMP>& _series,
					      double bound)
  : PosNegSum<Accelerator>(_series, bound)
{
  SeqUtils::combine_positive_negative(PosNegSum<Accelerator>::real().pos(),
				      PosNegSum<Accelerator>::real().neg(),
				      real_combined_);
  SeqUtils::combine_positive_negative(PosNegSum<Accelerator>::imag().pos(),
				      PosNegSum<Accelerator>::imag().neg(),
				      imag_combined_);
}

template<class Accelerator>
CMP CombinedRealSum<Accelerator>::sum()
{
  const double realSum = Accelerator(real_combined_).sum();
  const double imagSum = Accelerator(imag_combined_).sum();
  return CMP(realSum, imagSum);
}
