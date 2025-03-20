#ifndef _LOGGING_LOGGING_H_
#define _LOGGING_LOGGING_H_

#include <functional>
#include <ostream>
#include <streambuf>
#include <string>
#include "log_stream.h"

namespace logging {


enum LogLevel
{
    LOG_INNER_DEBUG = 0,    // This option is used for internal debugging purposes
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    NUM_LOG_LEVELS,
};


/*
 * @note  If roll_cycle_minutes is equal to 0, no new log files will be
 * generated based on time rolling. If roll_size_bytes is equal to 0, new log
 * files will not be rolled based on the log file size.
 */
typedef struct LogControl
{
    bool use_ms;                    // If true, millisecond precision is used, otherwise second precision is used
    bool show_path;                 // Whether to display the file and line number of the log
    bool show_func;                 // Whether to display the function to which the log belongs
    LogLevel level;                 // Only logs that are greater than or equal to that log level are displayed
    std::string logfile;            // The name of the log file
    uint64_t roll_cycle_minutes;    // Log file rolling period, in minutes.
    uint64_t roll_size_kbytes;       // Log File rolling size, in Kbytes
}LogContorl;


/**
 * @brief Logger class, implemented based on asynchronous logger, and provides
 * C++ stream style output log
 */
class Logger
{
public:
    /**
     * @brief Logger constructor, each message instantiates a logger
     * @param [in] level : The current level of this log message
     * @param [in] file : The file where this current log message is located
     * @param [in] func_name : The function where this current log message is
     * located
     * @param [in] line : The line number of the current log message
     */
    Logger(const LogLevel level, const char *file, const char *func_name, const size_t line, bool show_header = true);
    /**
     * @brief Logger destructor, execute log data refresh when destructed
     */
    ~Logger();

    /**
     * @brief Get the logger's streaming buffer.
     */
    LogStream &stream ()
    {
        return *_stream;
    }

private:
    LogStream *_stream;
}; // class Logger

/**
 * @brief Log module initialization
 * @param [in] cfg : Log control parameters

 */
void log_init (LogContorl cfg);

/* Global log level */
extern LogLevel _global_log_level;

} // namespace logging

#ifdef DISABLE_LOG
    #define _LOG(LEVEL)                                                           \
        if ((logging::LOG_##LEVEL > logging::NUM_LOG_LEVELS) && (logging::LOG_##LEVEL < logging::NUM_LOG_LEVELS)) \
        logging::Logger(logging::LOG_##LEVEL, __FILE__, __func__, __LINE__).stream()
    #define _LOG_RAW(LEVEL)                                                           \
        if ((logging::LOG_##LEVEL >= NUM_LOG_LEVELS) && (logging::LOG_##LEVEL < logging::NUM_LOG_LEVELS)) \
        logging::Logger(logging::LOG_##LEVEL, __FILE__, __func__, __LINE__, false).stream()
#else
    #define _LOG(LEVEL)                                                           \
        if ((logging::LOG_##LEVEL >= logging::_global_log_level) && (logging::LOG_##LEVEL < logging::NUM_LOG_LEVELS)) \
        logging::Logger(logging::LOG_##LEVEL, __FILE__, __func__, __LINE__).stream()

    #define _LOG_RAW(LEVEL)                                                           \
        if ((logging::LOG_##LEVEL >= logging::_global_log_level) && (logging::LOG_##LEVEL < logging::NUM_LOG_LEVELS)) \
        logging::Logger(logging::LOG_##LEVEL, __FILE__, __func__, __LINE__, false).stream()
#endif

#define LOG(LEVEL) _LOG(LEVEL)
#define LOG_RAW(LEVEL)  _LOG_RAW(LEVEL)

#endif // _LOGGING_LOGGING_H_