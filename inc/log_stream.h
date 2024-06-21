#ifndef _LOGGING_LOG_STREAM_H_
#define _LOGGING_LOG_STREAM_H_

#include <ostream>
#include <streambuf>
#include <functional>
namespace logging {
/**
* @brief LogStream：日志流式缓冲类。继承 std::ostream 实现c++的流式输出，继承 std::streambuf 实现缓存
*/
class LogStream : virtual public std::streambuf, public std::ostream{

public:

/**
* @brief LogStream：继承 std::ostream 实现c++的流式输出，继承 std::streambuf 实现缓存
* @param [in] buffer_size : 缓存大小
*/
LogStream(size_t buffer_size);

/**
* @brief LogStream 析构时释放持有的内存
*/
~LogStream();

/**
* @brief streambuffer满时，增大缓冲区
* @param [in] c : 溢出的字符
*/
virtual int overflow(int c);

/**
* @brief 刷新缓冲区，将数据输出到异步日志输出接口中
*/
void flush_data(void);

/**
* @brief 重置输出缓冲区
*/
void reset_buffer(void);

/**
* @brief 刷新缓冲区，将数据输出到异步日志输出接口中
*/
static void set_output_func(std::function<void(const char*, size_t)> f);

private:
char *_buffer;
size_t _size;
static std::function<void(const char*, size_t)> _output_func;

}; // class LogStream

} // namespace logging

#endif // _LOGGING_LOG_STREAM_H_