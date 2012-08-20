#include <fstream>
#include "csutils/logger.h"

#ifdef USE_BOOST_LOGGER
#include <logging.hpp>
namespace Logger {

  void init(const std::string& logFile)
  {
    namespace blog = boost::logging;
    static blog::filename_element fil_el(blog::filename_element::filename_only);
    blog::format log_format("[" >>
			    fil_el >> ":" >>
			    blog::line >> "] " >>
			    blog::trace >>
			    blog::eol);
    BOOST_LOG_INIT(log_format);
    blog::sink sink(new std::ofstream(logFile.c_str()),
		    BOOST_LOG_MASK_LEVEL_2);
    sink.attach_qualifier(blog::log);
    BOOST_LOG_ADD_OUTPUT_STREAM(sink);
  }
} // namespace Logger

#else

#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
namespace Logger {

  boost::shared_ptr<std::ofstream> log_;

  void init(const std::string& logFile)
  {
    if(log_.get()) {
      log_->close();
    }
    log_.reset(new std::ofstream(logFile.c_str()));
  }

  void log(const std::string& file, int line, const std::string& msg)
  {
    if (log_.get()) {
      (*log_) << boost::format("[%1%:%2%] %3%\n") % file % line % msg;
    }
  }
} // namespace Logger
#endif

