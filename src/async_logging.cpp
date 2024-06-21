#include <cstring>
#include <iostream>
#include <chrono>
#include "async_logging.h"
#include "fast_memcpy.h"

namespace logging{

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
void AsyncLogging::init(std::string file_name, uint64_t roll_cycle_minutes, uint64_t roll_size_bytes) {

_log_file_ptr.reset(new LogFile(file_name, roll_cycle_minutes, roll_size_bytes));
if (nullptr == _log_file_ptr) {
std::cerr << "!!!!! insufficient memory resources !!!!!\n";
return ;
}
/* 初始状态下，共有 11 个空闲buffer可用，有数据的buffer为0 */
_input_queue_ptr = std::unique_ptr<BufferQueue>(new BufferQueue(4));
_output_queue_ptr = std::unique_ptr<BufferQueue>(new BufferQueue(0));
_cur_buffer_ptr = std::unique_ptr<DataBuffer>(new DataBuffer());

_running = false;
}

/**
* @brief 异步日志类析构，析构时需要检查是否还有缓存的日志数据没写到buffer中
*/
AsyncLogging::~AsyncLogging(void) {

_running = false;

/* 清空队列中的缓存 */
while (!_output_queue_ptr->empty()) {
auto tmp = _output_queue_ptr->pop_buffer(1);
if (nullptr != tmp) {
_log_file_ptr->append_data(tmp->_buffer, tmp->_cur_size);
tmp->_cur_size = 0;
}
}
/* 当前持有的buffer也可能会有数据没写入 */
std::unique_lock<std::mutex> lock(_buffer_lock);
if (_cur_buffer_ptr->_cur_size > 0) {
_log_file_ptr->append_data(_cur_buffer_ptr->_buffer, _cur_buffer_ptr->_cur_size);
_cur_buffer_ptr->_cur_size = 0;
}
lock.unlock();

_log_file_ptr->flush();

if(_background_thread.joinable()) {
/* 后台消费线程等 queue的超时是 1s， 所以程序退出是差不多有 1s 的延迟*/
_background_thread.join();
}
}

/**
* @brief 日志写入缓存
* @param [in] data : 日志数据
* @param [in] size : 写入日志字节长度
*/
void AsyncLogging::append_data(const char *data, size_t size){

std::unique_lock<std::mutex> lock(_buffer_lock);
if (_cur_buffer_ptr->_cur_size + size > _cur_buffer_ptr->BUFFER_SIZE) {
_output_queue_ptr->push_buffer(_cur_buffer_ptr);

_cur_buffer_ptr = _input_queue_ptr->pop_buffer(1000);
// 日志输出太快了，下游处理不过来，没有空间buffer可以写入日志数据，这部分数据直接丢弃了
if (nullptr == _cur_buffer_ptr) {
std::cerr << "\n!!!!!!!!!!!!!Log input is too fast!!!!!!!!!!!!!\n";
}
}

if (_cur_buffer_ptr != nullptr) {
memcpy_fast(_cur_buffer_ptr->_buffer+_cur_buffer_ptr->_cur_size, data, size);
_cur_buffer_ptr->_cur_size += size;
}
}

/**
* @brief 后台消费线程，AsyncLogging创建时启动该任务，后台消费线程负责将日志数据写入日志文件中。
*/
void AsyncLogging::background_consume_thread(void) {

while (_running) {
DataBuffer_ptr buffer_ptr = _output_queue_ptr->pop_buffer(1000);
if(nullptr != buffer_ptr) {

/* 从队列中获取到了buffer，就将数据写入文件*/
_log_file_ptr->append_data(buffer_ptr->_buffer, buffer_ptr->_cur_size);

/* 数据写完后，buffer返还到空闲queue中 */
buffer_ptr->_cur_size = 0;
_input_queue_ptr->push_buffer(buffer_ptr);
} else {
/* 队列中没有写满数据的buffer，那就将当前_cur_buffer_ptr指向的Buffer中的数据写入日志文件 */
std::unique_lock<std::mutex> lock(_buffer_lock);
DataBuffer_ptr tmp = std::move(_cur_buffer_ptr);
_cur_buffer_ptr = _input_queue_ptr->pop_buffer(1);
lock.unlock();

if (nullptr != tmp && tmp->_cur_size > 0) {
_log_file_ptr->append_data(tmp->_buffer, tmp->_cur_size, true);
tmp->_cur_size = 0;
// 空闲buffer返还到队列中
_input_queue_ptr->push_buffer(tmp);
}
}
}

}

/**
* @brief 启动日志记录
*/
void AsyncLogging::start() {
_running = true;
std::thread t(&AsyncLogging::background_consume_thread, this);
_background_thread = std::move(t);

/* 等待后台线程实际启动 */
while(!_background_thread.joinable()){}
}

} // namespace logging
