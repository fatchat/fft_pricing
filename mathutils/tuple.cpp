#include <algorithm>
#include <stdexcept>
//#include <iostream>
#include "mathutils/tuple.h"

// TODO make non-recursive
size_t make_tuples_(TupleFactory<size_t>& tf,
		    TupleFactory<size_t>::tuple_type& partial,
		    size_t next_index, size_t N)
{
  size_t index = 0;
  if (next_index >= tf.size()) {
    index = tf.create_and_store_tuple();
    //std::cout << "created at " << index << std::endl;
    std::copy(partial.begin(), partial.end(), tf.get_iter(index));
  }
  else {
    for (size_t ii = 0; ii < N; ++ii) {
      partial[next_index] = ii;
      index = make_tuples_(tf, partial, 1 + next_index, N);
    }
  }
  return index;
}

std::pair<size_t, size_t>
generate_tuples_mod_N(TupleFactory<size_t> &tf, size_t N)
{
  if (N == 0) {
    throw std::invalid_argument("N must be > 0");
  }
  TupleFactory<size_t>::tuple_type empty = tf.make_tuple();
  const size_t last = make_tuples_(tf, empty, 0, N);
  const size_t first = last + 1 - pow(N, tf.size());
  return std::make_pair(first, last);
}
