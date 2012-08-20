#ifndef BASKET_H
#define BASKET_H

#include <iosfwd>
#include <boost/smart_ptr.hpp>

#include "csutils/globals.h"
#include "mathutils/linalg.h"
#include "mathutils/tuple.h"

class Basket {
 public:
  typedef LinAlgSys::vector_type       vector_type;
  typedef LinAlgSys::array_type        array_type;
  typedef LinAlgSys::c_array_type      c_array_type;
  typedef LinAlgSys::c_vector_type     c_vector_type;
  typedef TupleFactory<size_t>::tuple_type tuple_type;
  typedef std::pair<size_t, size_t>    range_type;
  typedef std::vector<vector_type>     stockvec_type;
  typedef boost::shared_ptr<Basket>    ptr_type;
 private:
  LinAlgSys las_;
  TupleFactory<size_t> tf_;
  const size_t S_index_;
  const size_t sigma_index_;
  const size_t rho_index_;
  const size_t X_index_;
  const size_t v_index_;
  const double K_;
  const double r_;
  const double time_;
  const double delta_;
  stockvec_type stockVector_;

  typedef CMP (*p_hat_type)(const c_vector_type&);

  vector_type make_v_() const;
  void make_A_(const range_type&, p_hat_type p_hat,
	       size_t N, const c_vector_type& e, c_vector_type& A) const;
  void make_C_(const range_type&,
	       size_t N, size_t b, const vector_type& e, c_vector_type& C,
	       const c_vector_type& B);
  void make_D_basket_(vector_type&, const c_vector_type&) const;
  void make_D_spread_(vector_type&, const c_vector_type&) const;
  CMP basketgamma_(const c_vector_type&, p_hat_type) const;
  void fftpricing_(const vector_type& e, size_t N, vector_type& D,
		   p_hat_type p_hat, c_vector_type& C);
  double Kdisc_() const { return exp (-r_ * time_) * K_; }
  double factor_(size_t b) const { return Kdisc_() * pow(0.5 * delta_ / PI, b); }
 public:
  Basket(const LinAlgSys& linAlgSys,
	 size_t S_index, size_t sigma_index, size_t rho_index,
	 double K, double r, double time);
  void basketpricingFFT(const vector_type& ee, size_t, vector_type&);
  void spreadpricingFFT(const vector_type& ee, size_t, vector_type&);
  const stockvec_type& stockVector() const { return stockVector_; }
  void showData(std::ostream&) const;
};

Basket::ptr_type create_basket(LinAlgSys& linalgsys);

#endif // BASKET_H

