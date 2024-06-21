#include <ios> // std::streamsize
#include <iostream>
#include <functional>
#include "async_logging.h"
#include "logging.h"
#include "fast_memcpy.h"
#include <sys/time.h>


namespace logging {

// 使用线程局部变量，多线程安全
thread_local std::time_t global_last_second = 0;
thread_local char global_time_str[30] = "2022-12-20 12:20:2";
thread_local LogStream global_log_stream(256);

const char* LogLevelName[NUM_LOG_LEVELS] =
{
"DEBUG: ",
"INFO: ",
"WARN: ",
"FATAL: ",
};

/**
* @brief 构建日志类，每条信息构建一个日志类。
* @param [in] level : 当前这条日志的等级
* @param [in] file : 当前这条日志所在的文件
* @param [in] func_name : 当前这条日志所在的函数名
* @param [in] line : 当前这条日志所在的行号
*/
Logger::Logger(const LogLevel level, const char *file, const char *func_name, const size_t line) {

_stream = &global_log_stream;
_stream->reset_buffer();
//(*_stream) << LogLevelName[level];

// std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
// if (now != global_last_second) {
// global_last_second = now;
// std::tm tm_data;
// // 该函数耗时
// localtime_r(&now, &tm_data);
// std::strftime(global_time_str, sizeof(global_time_str), "%Y-%m-%d %H:%M:%S", &tm_data);
// }
//(*_stream) << global_time_str;
//(*_stream) << file << line << func_name ;

// (*_stream) << " [" << file << ":" << line << " "<< func_name << "] ";
}

/**
* @brief 日志类析构函数，析构时执行日志数据刷新
*/
Logger::~Logger() {
if (nullptr != _stream) {
// (*_stream) << "\n";
(*_stream).flush_data();
}
}


/* Logger 全局变量初始化 */
LogLevel _global_log_level = LOG_DEBUG;
AsyncLogging _global_async_logging;

void async_output(const char *data, size_t size) {
_global_async_logging.append_data(data, size);
}

/**
* @brief 日志模块初始化
* @param [in] conf_file : 日志配置文件路径
*/
void log_init(std::string conf_file) {

LogStream::set_output_func(async_output);

_global_log_level = LOG_INFO;
_global_async_logging.init("test_time_cycle.log", 10, 0);

_global_async_logging.start();
}

} //namespace logging