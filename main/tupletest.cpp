#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#include <cstdlib>

#include "csutils/get_opt.h"
#include "mathutils/tuple.h"

void showHelp(const char* progname)
{
  std::cout <<"usage: " << progname << " [-h(elp)] [-v(erbose)] <N> <b>\n";
}

int main(int argc, char* argv[])
{
  Getopt getopt;
  getopt.addOption("help", Option::NO_ARG);
  getopt.addOption("verbose", Option::NO_ARG);
  if (getopt.processOpts(argc, argv)) {
    showHelp(argv[0]);
    return 1;
  }
  if (argc - getopt.first_non_opt() != 2) {
    showHelp(argv[0]);
    return 1;
  }
  if (getopt.getOption('h').is_set()) {
    showHelp(argv[0]);
    return 0;
  }
  const bool verbose = getopt.getOption('v').is_set();

  const size_t N = atoi(argv[getopt.first_non_opt()]);
  const size_t b = atoi(argv[1 + getopt.first_non_opt()]);

  TupleFactory<size_t> tf(b);
  const std::pair<size_t, size_t> ret = generate_tuples_mod_N(tf, N);

  if (verbose) {
    std::cout << "N=" << N << " b=" << b << "\n"
	      << "first=" << ret.first << " last=" << ret.second << std::endl;
  }

  for (size_t ii = ret.first; ii <= ret.second; ++ii) {
    const TupleFactory<size_t>::tuple_type tuple = tf.make_copy(ii);
    assert(tuple.size() == tf.size());
    std::cout << tuple << std::endl;
  }
  const size_t x = tf.create_and_store_tuple();
  const size_t y = ret.first;
  const TupleFactory<size_t>::tuple_type tuple_x = tf.make_copy(x);
  const TupleFactory<size_t>::tuple_type tuple_y = tf.make_copy(y);
  assert(tuple_x.size() == tf.size());
  assert(tuple_y.size() == tf.size());
  if (verbose) {
    std::cout << tuple_x << ' '
	      << tuple_y << ' '
	      << std::boolalpha << tf.isequal(x, y)
	      << std::endl;
  }
}
