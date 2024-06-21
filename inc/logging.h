#ifndef _LOGGING_LOGGING_H_
#define _LOGGING_LOGGING_H_

#include <string>
#include <ostream>
#include <streambuf>
#include <functional>
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
* @brief 日志类，基于异步日志实现日志类，提供C++ stream风格输出日志
*/
class Logger {
public:
/**
* @brief 构建日志类，每条信息构建一个日志类。
* @param [in] level : 当前这条日志的等级
* @param [in] file : 当前这条日志所在的文件
* @param [in] func_name : 当前这条日志所在的函数名
* @param [in] line : 当前这条日志所在的行号
*/
Logger(const LogLevel level, const char *file, const char *func_name, const size_t line);
/**
* @brief 日志类析构函数，析构时执行日志数据刷新
*/
~Logger();

/**
* @brief 获取日志对象的流式缓冲区。
*/
LogStream& stream() {return *_stream;}

private:
LogStream *_stream;
}; // class Logger

/**
* @brief 模块初始化
* @param [in] conf_file : 日志配置文件路径
*/
void log_init(std::string conf_file);

/* 全局日志等级 */
extern LogLevel _global_log_level;

} // namespace logging

#define _LOG(LEVEL) if((LOG_##LEVEL >= _global_log_level) && (LOG_##LEVEL < NUM_LOG_LEVELS)) \
logging::Logger(LOG_##LEVEL, __FILE__, __func__, __LINE__).stream()
#define LOG(LEVEL) _LOG(LEVEL)

#endif // _LOGGING_LOGGING_H_