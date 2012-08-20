#ifndef TUPLE_H
#define TUPLE_H

#include <boost/numeric/ublas/vector.hpp>
#include <utility>

template<class T>
class TupleFactory {
  const std::size_t size_;
 public:
  typedef boost::numeric::ublas::vector<T>     tuple_type;
  typedef typename tuple_type::iterator        iterator;
  typedef typename tuple_type::const_iterator  const_iterator;
 private:
  tuple_type tuples_;
  size_t next_index_;
 public:
  TupleFactory(std::size_t size) : size_(size), next_index_(0) { }
  size_t size() const { return size_; }
  tuple_type make_tuple() const
  { return tuple_type(size_, T()); }
  iterator get_iter(size_t index)
  { return tuples_.begin() + index * size_; }
  const_iterator get_const_iter(size_t index) const
  { return tuples_.begin() + index * size_; }
  size_t create_and_store_tuple();
  tuple_type make_copy(size_t index) const;
  bool isequal(size_t, size_t) const;
}; // TupleFactory

std::pair<size_t, size_t>
  generate_tuples_mod_N(TupleFactory<size_t> &tf, size_t N);

#include "mathutils/tuple.hpp"

#endif // TUPLE_H
