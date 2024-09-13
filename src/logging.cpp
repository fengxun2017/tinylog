#include "logging.h"
#include <sys/time.h>
#include <functional>
#include <ios>  // std::streamsize
#include <iostream>
#include "async_logging.h"
#include "fast_memcpy.h"

namespace logging {

void async_output(const char *data, size_t size);

/* Global log level */
LogLevel _global_log_level = LOG_DEBUG;
AsyncLogging _global_async_logging;

/* Use thread local variables, multi-thread safe */
thread_local std::time_t global_last_second = 0;
thread_local char global_time_str[] = "2024-01-01 00:00:00";
thread_local LogStream global_log_stream(128, async_output);

const char *LogLevelName[NUM_LOG_LEVELS] = {
    "DEBUG: ",
    "INFO: ",
    "WARN: ",
    "ERROR: ",
};

 /**
* @brief Logger constructor, each message instantiates a logger
* @param [in] level : The current level of this log message
* @param [in] file : The file where this current log message is located
* @param [in] func_name : The function where this current log message is located
* @param [in] line : The line number of the current log message
*/
Logger::Logger(const LogLevel level, const char *file, const char *func_name,
               const size_t line) {

    _stream = &global_log_stream;
    _stream->reset_buffer();
    //(*_stream) << LogLevelName[level];

    // std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // if (now != global_last_second) {
    // global_last_second = now;
    // std::tm tm_data;

    // localtime_r(&now, &tm_data);
    // std::strftime(global_time_str, sizeof(global_time_str), "%Y-%m-%d %H:%M:%S", &tm_data);
    // }
    //(*_stream) << global_time_str;
    //(*_stream) << file << line << func_name ;

    // (*_stream) << " [" << file << ":" << line << " "<< func_name << "] ";
}

/**
* @brief Logger destructor, execute log data refresh when destructed
*/
Logger::~Logger() {
    if (nullptr != _stream) {
        // (*_stream) << "\n";
        (*_stream).flush_data();
    }
}

void async_output(const char *data, size_t size) {
    _global_async_logging.append_data(data, size);
}


/**
* @brief Log module initialization
* @param [in] conf_file : Log configuration file path
*/
void log_init(std::string conf_file) {

    _global_log_level = LOG_INFO;
    _global_async_logging.init("test_time_cycle.log", 10, 0);

    _global_async_logging.start();
}

}  //namespace logging