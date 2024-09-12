#ifndef _LOGGING_LOGGING_H_
#define _LOGGING_LOGGING_H_

#include <functional>
#include <ostream>
#include <streambuf>
#include <string>
#include "log_stream.h"

namespace logging {

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    NUM_LOG_LEVELS,
};

/**
* @brief Logger class, implemented based on asynchronous logger, and provides C++ stream style output log
*/
class Logger {
   public:
    /**
* @brief Logger constructor, each message instantiates a logger
* @param [in] level : The current level of this log message
* @param [in] file : The file where this current log message is located
* @param [in] func_name : The function where this current log message is located
* @param [in] line : The line number of the current log message
*/
    Logger(const LogLevel level, const char *file, const char *func_name,
           const size_t line);
    /**
* @brief Logger destructor, execute log data refresh when destructed
*/
    ~Logger();

    /**
* @brief Get the logger's streaming buffer.
*/
    LogStream &stream() { return *_stream; }

   private:
    LogStream *_stream;
};  // class Logger

/**
* @brief Log module initialization
* @param [in] conf_file : Log configuration file path
*/
void log_init(std::string conf_file);

/* Global log level */
extern LogLevel _global_log_level;

}  // namespace logging

#define _LOG(LEVEL)                                                           \
    if ((LOG_##LEVEL >= _global_log_level) && (LOG_##LEVEL < NUM_LOG_LEVELS)) \
    logging::Logger(LOG_##LEVEL, __FILE__, __func__, __LINE__).stream()
#define LOG(LEVEL) _LOG(LEVEL)

#endif  // _LOGGING_LOGGING_H_