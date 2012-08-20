#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace Logger {
  void init(const std::string& logFile);
#ifndef USE_BOOST_LOGGER
  void log(const std::string& file, int line, const std::string& msg);
#endif
}

#ifdef USE_BOOST_LOGGER
#include <logging.hpp>
#define logthis( s ) BOOST_LOG_(1, s)
#else
#define logthis( s ) Logger::log(__FILE__, __LINE__, s)
#endif

#define LOG(...) \
  { \
    std::ostringstream ostr; \
    ostr << __VA_ARGS__; \
    logthis(ostr.str()); \
  }

#define LOGVECTOR(a, b, c)			\
  { \
    std::ostringstream ostr; \
    ostr << a; \
    std::copy(b.begin(), b.end(), std::ostream_iterator<c>(ostr, ", ")); \
    logthis(ostr.str()); \
  }

#endif // LOGGER_H
