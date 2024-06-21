#ifndef _LOGGING_ASYNC_LOGGING_H_
#define _LOGGING_ASYNC_LOGGING_H_

#include <mutex>
#include <memory>
#include <thread>
#include "log_file.h"
#include "buffer_queue.h"

namespace logging {
/**
* @brief 异步日志类，支持多线程写入日志。
* 内部实际由两个 queue组成， 面向输入的 input_queue，和面向输出的 output_queue
* 日志写入时，从input_queue中获取空闲的buffer_ptr，将日志信写入buffer中，再将该buffer放入 output_queue。
* 后台异步任务(只有一个)，会不停从output_queue中取有日志数据的 buffer_ptr, 将日志数据写入文件，再将该buffer_ptr放回input_queue，供下次写入
*/ 

class AsyncLogging {

public:
/**
* @brief 异步日志类构造函数
* @param [in] file_name: 日志文件名
* @param [in] roll_cycle_hours: 文件滚动周期，分钟为单位。
等于0，不会根据时间进行滚动生成新日志文件。
大于0，每roll_cycle个小时会生成新的日志文件。
* @param [in] roll_size_bytes: 文件滚动大小，字节为单位
等于0，不会根据日志文件大小滚动生成新日志文件。
大于0，当前日志文件写入字节大于该值时，自动创建新日志文件。
*/
void init(std::string file_name, uint64_t roll_cycle_minutes=0, uint64_t roll_size_bytes=0);

/**
* @brief 异步日志类析构，析构时需要检查是否还有缓存的日志数据没写到buffer中
*/
~AsyncLogging(void);

AsyncLogging(void) : _cur_buffer_ptr(nullptr){}
/**
* @brief 日志写入
* @param [in] data : 日志数据
* @param [in] size : 写入日志字节长度
*/
void append_data(const char *data, size_t size);

/**
* @brief 启动日志记录
*/
void start();

/**
* @brief 后台消费线程，AsyncLogging创建时启动改任务，后台消费线程负责将日志数据写入日志文件中。
*/
void background_consume_thread(void);

private:
DataBuffer_ptr _cur_buffer_ptr; ///< 当前正在使用的buffer
std::mutex _buffer_lock; ///< 保证对 _cur_buffer_ptr 访问的线程安全

std::unique_ptr<BufferQueue> _input_queue_ptr; ///< 输入Queue，从中获取空闲buffer，填满日志数据后放入 输出Queue中，
std::unique_ptr<BufferQueue> _output_queue_ptr; ///< 输出Queue，从中获取有数据的buffer，数据写入日志文件后，再将空buffer放回 输入Queue。

bool _running; ///< 后台消费线程运行指示
std::thread _background_thread; ///< 后台消费线程
std::unique_ptr<LogFile> _log_file_ptr; ///< 消费线程实际写入的日志文件 

};

} // namespace logging
#endif // _LOGGING_ASYNC_LOGGING_H_