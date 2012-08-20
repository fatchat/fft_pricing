//#include <iostream>
#include <algorithm>
#include <stdexcept>

template<class T>
size_t TupleFactory<T>::create_and_store_tuple()
{
  const size_t retVal = next_index_++;
  if (size_ * next_index_ >= tuples_.size()) {
    //std::cout << "resizing from " << tuples_.size();
    tuples_.resize(2 * size_ * next_index_, true);
    //std::cout << " to " << tuples_.size() << std::endl;
  }
  return retVal;
}

template<class T>
typename TupleFactory<T>::tuple_type
TupleFactory<T>::make_copy(size_t index) const
{
  if(index >= next_index_) {
    throw std::range_error("tried to access tuple past range");
  }
  tuple_type retval = make_tuple();
  const_iterator start = get_const_iter(index);
  std::copy(start, start + size_, retval.begin());
  return retval;
}

template<class T>
bool TupleFactory<T>::isequal(size_t lhsindex, size_t rhsindex) const
{
  if (lhsindex == rhsindex) { return true; }
  const const_iterator lhsiter = get_const_iter(lhsindex);
  const const_iterator rhsiter = get_const_iter(rhsindex);
  for (size_t ii = 0; ii < size_; ++ii) {
    if (*(lhsiter + ii) != *(rhsiter + ii)) {
      return false;
    }
  }
  return true;
}

