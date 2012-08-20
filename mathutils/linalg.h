#ifndef LINALG_H
#define LINALG_H

#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include "csutils/globals.h"

class LinAlgSys {
  std::size_t size_;

 public:
  typedef boost::numeric::ublas::vector<double>  vector_type;
  typedef boost::numeric::ublas::matrix<double>  array_type;
  typedef boost::numeric::ublas::vector<CMP>     c_vector_type;
  typedef boost::numeric::ublas::matrix<CMP>     c_array_type;

 public:
  LinAlgSys(std::size_t size)
    : size_(size) { }
  std::size_t size() const { return size_; }
  vector_type make_vector() const
  { return vector_type(size_); }
  c_vector_type make_c_vector() const
  { return c_vector_type(size_); }
  array_type make_array() const
  { return array_type(size_, size_); }
  vector_type zero_vector() const
  { return boost::numeric::ublas::zero_vector<double>(size_); }
  vector_type scalar_vector(double val) const
  { return boost::numeric::ublas::scalar_vector<double>(size_, val); }
  c_vector_type scalar_vector(CMP val) const
  { return boost::numeric::ublas::scalar_vector<CMP>(size_, val); }

 public:
  typedef boost::shared_ptr<vector_type>   ptr_vector_type;
  typedef boost::shared_ptr<array_type>    ptr_array_type;
  typedef boost::shared_ptr<c_vector_type> ptr_c_vector_type;

 private:
  std::vector< ptr_vector_type >           vectors_;
  std::vector< ptr_array_type >            arrays_;
  std::vector< ptr_c_vector_type >         c_vectors_;

 public:
  std::size_t create_and_store_vector()
  { vectors_.push_back(ptr_vector_type(new vector_type(size_)));
    return vectors_.size() - 1; }
  std::size_t create_and_store_array()
  { arrays_.push_back(ptr_array_type(new array_type(size_, size_)));
    return arrays_.size() - 1; }
  std::size_t create_and_store_c_vector()
  { c_vectors_.push_back(ptr_c_vector_type(new c_vector_type(size_)));
    return c_vectors_.size() - 1; }

  vector_type& access_vector_ref(std::size_t index)
    { return *(vectors_.at(index)); }
  array_type& access_array_ref(std::size_t index)
    { return *(arrays_.at(index)); }
  c_vector_type& access_c_vector_ref(std::size_t index)
    { return *(c_vectors_.at(index)); }

  const vector_type& access_vector(std::size_t index) const
    { return *(vectors_.at(index)); }
  const array_type& access_array(std::size_t index) const
    { return *(arrays_.at(index)); }
  const c_vector_type& access_c_vector(std::size_t index) const
    { return *(c_vectors_.at(index)); }


}; // LinAlgSys

#endif // LINALG_H
