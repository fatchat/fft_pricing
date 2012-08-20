#include <cmath>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#include <boost/format.hpp>
#include <fftw3.h>

#include "mathutils/gamma.h"
#include "mathutils/tuple.h"
#include "pricing/basket.h"
#include "csutils/logger.h"

// ===========================================================================
// ft of the basket payoff function
CMP P_hat_basket(const Basket::c_vector_type& u)
{
  const CMP sum = std::accumulate(u.begin(), u.end(), CMP(0,0));
  CMP lognum(0,0);
  for(size_t j = 0; j < u.size(); ++j) {
    lognum += Gamma::log_gsl_gamma(-c_I * u[j]);
  }
  const CMP logden = Gamma::log_gsl_gamma(-c_I * sum + CMP(2,0));
  return exp(lognum - logden);
}

// ===========================================================================
// ft of the spread payoff function
CMP P_hat_spread(const Basket::c_vector_type& u)
{
  assert(u.size() == 2);
  const CMP lognum = Gamma::log_gsl_gamma(c_I * (u[0] + u[1]) - 1.0)
    + Gamma::log_gsl_gamma(-c_I * u[1]);
  const CMP logden = Gamma::log_gsl_gamma(c_I * u[0] + 1.0);
  return exp(lognum - logden);
}

// ===========================================================================
Basket::Basket(const LinAlgSys& linAlgSys,
	       size_t S_index, size_t sigma_index, size_t rho_index,
	       double K, double r, double time)
  : las_(linAlgSys)
  , tf_(las_.size())
  , S_index_(S_index)
  , sigma_index_(sigma_index)
  , rho_index_(rho_index)
  , X_index_(las_.create_and_store_vector())
  , v_index_(las_.create_and_store_vector())
  , K_(K), r_(r), time_(time)
  , delta_(0.25)
{
  vector_type& X = las_.access_vector_ref(X_index_);
  const vector_type T = las_.access_vector(S_index_) / K_;
  std::transform(T.begin(), T.end(), X.begin(), &log);
  las_.access_vector_ref(v_index_) = make_v_();
  array_type& rho = las_.access_array_ref(rho_index_);
  const vector_type& sigma = las_.access_vector(sigma_index_);
  for (size_t ii = 0; ii < las_.size(); ++ii)
    for (size_t jj = 0; jj < las_.size(); ++jj)
      {
	rho(ii, jj) *= sigma[ii] * sigma[jj];
      }
}

// ===========================================================================
Basket::vector_type Basket::make_v_() const
{
  vector_type v = las_.scalar_vector(r_ * time_);
  vector_type sigma = las_.access_vector(sigma_index_);
  std::transform(sigma.begin(), sigma.end(), sigma.begin(), sigma.begin(),
		 std::multiplies<double>());
  sigma *= time_ / 2.0;
  v -= sigma;
  return v;
}

// ===========================================================================
CMP Basket::basketgamma_(const c_vector_type& u, p_hat_type p_hat) const
{
  const vector_type& v = las_.access_vector(v_index_);
  const CMP fs = boost::numeric::ublas::inner_prod(u, v);
  const CMP a = c_I * fs;
  const c_vector_type rho_u = prod(las_.access_array(rho_index_), u);
  const CMP mm = boost::numeric::ublas::inner_prod(rho_u, u);
  const CMP b = mm * 0.5 * time_;
  const CMP phi = exp(a - b);			
  const CMP phat = (*p_hat)(u);
  const CMP retVal = phi * phat;
#if 0
  {
    boost::format formatter("fs: %1%, a: %2%, b: %3%, phi: %4%, ss: %5%, phat: %6%, retval: %7%");
    formatter % fs % a % b % phi % ss % phat % retVal;
    logthis(formatter.str());
  }
#endif
  return retVal;
}

// ===========================================================================
void
perform_fft(Basket::c_vector_type&, Basket::c_vector_type&, size_t, size_t);

// ===========================================================================
void Basket::basketpricingFFT(const vector_type& e, size_t N, vector_type& D)
{
  c_vector_type C;
  fftpricing_(e, N, D, &P_hat_basket, C);
  make_D_basket_(D, C);
}

// ===========================================================================
void Basket::spreadpricingFFT(const vector_type& e, size_t N, vector_type& D)
{
  c_vector_type C;
  fftpricing_(e, N, D, &P_hat_spread, C);
  make_D_spread_(D, C);
}

// ===========================================================================
void Basket::fftpricing_(const vector_type& e, size_t N, vector_type& D,
			 p_hat_type p_hat, c_vector_type& C)
{
  const range_type range = generate_tuples_mod_N(tf_, N);
  const size_t b = las_.size();
  assert(range.second - range.first + 1 == pow(N, b));

  c_vector_type A;
  make_A_(range, p_hat, N, e, A);

#if 0
  LOGVECTOR("A: ", A, CMP);
#endif

  c_vector_type B;
  perform_fft(A, B, b, N);

#if 0
  LOGVECTOR("B: ", B, CMP);
#endif

  make_C_(range, N, b, e, C, B);
}

// ===========================================================================
void Basket::make_A_(const range_type& range, p_hat_type p_hat,
		     size_t N, const c_vector_type& e,
		     c_vector_type& A) const
{
  A.resize(range.second - range.first + 1);
  const vector_type& X = las_.access_vector(X_index_);
  const c_vector_type i_e = e * c_I;
  const vector_type minus_half_N = las_.scalar_vector(-int(N) * 0.5);
#if 0
  LOG("delta: " << delta_ << "; -N/2: " << minus_half_N);
#endif
  for(size_t ii = range.first; ii <= range.second; ++ii) {
    const tuple_type k_tuple = tf_.make_copy(ii);
#if 0
    LOG("k: " << k_tuple << "u: " << u_tuple);
#endif
    const c_vector_type u_tuple = (k_tuple + minus_half_N) * delta_ + i_e;
    const CMP ip = inner_prod(k_tuple, X);
    const CMP exp_ip = exp(c_I * delta_ * ip);

    A[ii - range.first] = exp_ip * basketgamma_(u_tuple, p_hat);
  }  
}

// ===========================================================================
// this is generic enough to eventually be moved to mathutils
void
perform_fft(Basket::c_vector_type& A, Basket::c_vector_type& B,
	    size_t b, size_t N)
{
  B.resize(A.size());
  const std::vector<int> ft_dims(b, N);
  fftw_complex *in = reinterpret_cast<fftw_complex*>(&A[0]);
  fftw_complex *out = reinterpret_cast<fftw_complex*>(&B[0]);
  fftw_plan plan
    = fftw_plan_dft(b, &ft_dims[0], in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
}

// ===========================================================================
void Basket::make_C_(const range_type& range,
		     size_t N, size_t b, const vector_type& e,
		     c_vector_type& C, const c_vector_type& B)
{
  C.resize(B.size());
  stockVector_.resize(C.size());

  const double lambda = 2 * PI / N / delta_;

  const c_vector_type vec
    = las_.scalar_vector(-c_I * (delta_ * N * 0.5)) - e;

  const vector_type& X = las_.access_vector(X_index_);

  for (size_t ii = range.first; ii <= range.second; ++ii) {
    const tuple_type tuple = tf_.make_copy(ii);
    const vector_type x_tuple = X + tuple * lambda;
    const CMP expV = exp(inner_prod(vec, x_tuple));
    C[ii - range.first] = B[ii - range.first] * expV;
    // set stockVector_
    vector_type& s = stockVector_[ii - range.first];
    s.resize(b);
    std::transform(x_tuple.begin(), x_tuple.end(), s.begin(), &exp);
    s *= K_;
  }
}

// ===========================================================================
void Basket::make_D_basket_(vector_type& D, const c_vector_type& C) const
{
  D.resize(C.size());
  const size_t b = las_.size();
  const vector_type& S = las_.access_vector(S_index_);
  const double S_sum = std::accumulate(S.begin(), S.end(), 0.0);
  const double factor = factor_(b);
  for (size_t ii = 0; ii < C.size(); ++ii) {
    D[ii] = S_sum - Kdisc_() + C[ii].real() * factor;
  }
}

void Basket::make_D_spread_(vector_type& D, const c_vector_type& C) const
{
  D.resize(C.size());
  const size_t b = las_.size();
  const double factor = factor_(b);
  for (size_t ii = 0; ii < C.size(); ++ii) {
    D[ii] = C[ii].real() * factor;
  }
}

// ===========================================================================
void Basket::showData(std::ostream& os) const
{
  os << "S: " << las_.access_vector(S_index_)
     << " sigma: " << las_.access_vector(sigma_index_)
     << " rho: " << las_.access_array(rho_index_)
     << " X: " << las_.access_vector(X_index_)
     << " v: " << las_.access_vector(v_index_)
     << " K: " << K_
     << " r_: " << r_
     << " time_: " << time_
     << " delta_: " << delta_
     << std::endl;
}

// ===========================================================================
#include "csutils/configfile.h"

Basket::ptr_type create_basket(LinAlgSys& linalgsys)
{
  const std::size_t size = linalgsys.size();
  const size_t Sindex = linalgsys.create_and_store_vector();
  {
    LinAlgSys::vector_type& stocks = linalgsys.access_vector_ref(Sindex);
    for (size_t ii = 0; ii < size; ++ii) {
      const std::string name = std::string("S_") + toString<int>(ii);
      stocks[ii] = get_value_from_config_file(name, 100);
    }
  }
  const size_t sigmaIndex = linalgsys.create_and_store_vector();
  {
    LinAlgSys::vector_type& sigma = linalgsys.access_vector_ref(sigmaIndex);
    for (size_t ii = 0; ii < size; ++ii) {
      const std::string name = std::string("sigma_") + toString<int>(ii);
      sigma[ii] = get_value_from_config_file(name, 0.05);
    }
  }
  const size_t rhoindex = linalgsys.create_and_store_array();
  {
    LinAlgSys::array_type& rho = linalgsys.access_array_ref(rhoindex);
    for (size_t ii = 0; ii < size; ++ii) {
      const std::string name_ii = std::string("rho_") + toString<int>(ii);
      for (size_t jj = 0; jj < size; ++jj) {
	const std::string name = name_ii + "_" + toString<int>(jj);
	rho(ii,jj) = get_value_from_config_file(name, 0.0);
      }
    }
  }

  const double K = get_value_from_config_file<double>("K", size * 100);
  const double r = get_value_from_config_file("r", 0.09);
  const double time = get_value_from_config_file("time", 1);

  return Basket::ptr_type(new Basket(linalgsys,
				     Sindex, sigmaIndex, rhoindex,
				     K, r, time));
}
