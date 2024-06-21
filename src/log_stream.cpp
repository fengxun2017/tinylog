#include <iostream>
#include <streambuf>
#include "fast_memcpy.h"
#include "log_stream.h"

namespace logging {

/* 类静态成员初始化 */
std::function<void(const char*, size_t)> LogStream::_output_func = nullptr;

/**
* @brief LogStream：继承 std::ostream 实现c++的流式输出，继承 std::streambuf 实现缓存
* @param [in] buffer_size : 缓存大小
*/
LogStream::LogStream(size_t buffer_size) 
: std::ostream(this){
// 设置 streambuf 
_buffer = new (std::nothrow) char[buffer_size];
if (nullptr == _buffer) {
setp(nullptr, nullptr);
_size = 0;
} else {
_size = buffer_size;
setp(_buffer, _buffer + _size);
}
}

/**
* @brief LogStream 析构时释放持有的内存
*/
LogStream::~LogStream() {
if (nullptr != _buffer) {
free(_buffer);
// reset pointer
setp(nullptr, nullptr);
}
}

/**
* @brief streambuffer满时，增大缓冲区
* @param [in] c : 溢出的字符
*/
int LogStream::overflow(int c) {
std::cerr << "overflow\n";
// 当前使用的 streambuf 不够了，扩大一下
size_t new_size = (_size+1)*3/2;
char *new_buffer = new (std::nothrow) char[new_size];
if (nullptr == new_buffer) {
setp(NULL, NULL);
free(_buffer);
return std::streambuf::traits_type::eof();
} 
// 原数据拷贝到新内存
memcpy_fast(new_buffer, _buffer, _size);
free(_buffer);
_buffer = new_buffer;
// 设置新缓冲区的数据区间
setp(_buffer, _buffer+new_size);
// 移动当前写指针到旧数据之后
pbump(_size);
_size = new_size;

// 溢出的字符写到新缓冲区里
return sputc(c);
}

/**
* @brief 刷新缓冲区，将数据输出到异步日志输出接口中
*/
void LogStream::flush_data(void) {

// stream buffer 中有数据，就将数据输出到异步输出接口中
if (pbase() != pptr() && (nullptr != _output_func)) {
_output_func(pbase(), pptr() - pbase());
}
}

/**
* @brief 重置输出缓冲区
*/
void LogStream::reset_buffer(void) {
if (nullptr == _buffer) {
setp(nullptr, nullptr);
_size = 0;
} else {
setp(_buffer, _buffer + _size);
}
}

/**
* @brief 刷新缓冲区，将数据输出到异步日志输出接口中
*/
void LogStream::set_output_func(std::function<void(const char*, size_t)> f) {
_output_func = f;
}


} // namespace logging