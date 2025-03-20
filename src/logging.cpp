#include "logging.h"
#include <sys/time.h>
#include <functional>
#include <ios> // std::streamsize
#include <iostream>
#include <sstream>
#include <iomanip>
#include "async_logging.h"
#include "fast_memcpy.h"

namespace logging {

void async_output(const char *data, size_t size);

/* Global log level */
LogLevel     _global_log_level = LOG_INNER_DEBUG;
AsyncLogging _global_async_logging;
bool         _global_use_ms_precision = false; // By default, seconds precision is used
bool         _global_show_path = false;
bool         _global_show_func = false;


/* Use thread local variables, multi-thread safe */
thread_local std::time_t global_last_second = 0;
thread_local char        global_time_str[32]  = {0};
thread_local LogStream   global_log_stream(256, async_output);

const char *LogLevelName[NUM_LOG_LEVELS] = {
    "IDEBUG:",
    "DEBUG: ",
    "INFO : ",
    "WARN : ",
    "ERROR: ",
};

/**
 * @brief Logger constructor, each message instantiates a logger
 * @param [in] level : The current level of this log message
 * @param [in] file : The file where this current log message is located
 * @param [in] func_name : The function where this current log message is
 * located
 * @param [in] line : The line number of the current log message
 */
Logger::Logger(const LogLevel level, const char *file, const char *func_name, const size_t line, bool show_header)
{

    _stream = &global_log_stream;
    _stream->reset_buffer();

    if (show_header)
    {
        (*_stream) << LogLevelName[level] << "[ ";
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        if (time_t_now != global_last_second) {
            global_last_second = time_t_now;
            std::tm tm_data;

            localtime_r(&time_t_now, &tm_data);
            std::strftime(global_time_str, sizeof(global_time_str), "%Y-%m-%d %H:%M:%S", &tm_data);
        }
        (*_stream) << global_time_str;

        if (_global_use_ms_precision)
        {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            std::ostringstream oss;
            oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            (*_stream) << oss.str();
        }

        if (_global_show_path)
        {
            (*_stream) << " " << file << ":" << line;
        }

        if (_global_show_func)
        {
            (*_stream) << " " << func_name;
        }
        (*_stream) << " ] ";
    }
}

/**
 * @brief Logger destructor, execute log data refresh when destructed
 */
Logger::~Logger()
{
    if (nullptr != _stream)
    {
        // (*_stream) << "\n";
        (*_stream).flush_data();
    }
}

void
async_output (const char *data, size_t size)
{
    if (_global_async_logging.is_running())
    {
        _global_async_logging.append_data(data, size);
    }
    else
    {
        (void)fwrite(data, 1, size, stdout);
    }
}

/**
 * @brief Log module initialization
 * @param [in] conf_file : Log configuration file path
 */
void
log_init (LogContorl cfg)
{

    _global_use_ms_precision = cfg.use_ms; 
    _global_show_path = cfg.show_path;
    _global_show_func = cfg.show_func;
    _global_log_level = cfg.level;
    _global_async_logging.init(cfg.logfile, cfg.roll_cycle_minutes, cfg.roll_size_kbytes*1024);

    _global_async_logging.start();
}

} // namespace logging