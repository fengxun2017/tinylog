#ifndef _LOGGING_BUFFER_QUEUE_H_
#define _LOGGING_BUFFER_QUEUE_H_

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace logging {

/**
* @brief buffer数据结构，队列中存储的是指向该buffer结构的指针。
*/
class DataBuffer {
public:

DataBuffer(void) : _cur_size(0){}

static const size_t BUFFER_SIZE = 4*1024*1024;

size_t _cur_size; ///< 当前有的数据量
char _buffer[BUFFER_SIZE]; ///< 实际存储Buffer
};
using DataBuffer_ptr = std::unique_ptr<DataBuffer>;

/**
* @brief 内部使用，线程安全的buffer队列类。
* 队列中存储的元素为 buffer指针。每个 buffer指针 指向一块大小固定的缓存。
* @note 
*/ 
class BufferQueue
{
public:
/**
* @brief 创建Buffer队列
* @param [in] size : queue中存储的buffer个数,默认存储10个buffer
* @note queue中存储的是指向buffer的指针，即从Queue中获取空闲buffer时，返回的是buffer指针。
* buffer不需要使用了，放回Queue中时，也是push buffer指针。
*/
BufferQueue(uint32_t size = 10);

/**
* @brief 从queue中获取 buffer_ptr。
* @param [in] timeout_ms : 等待超时，单位为毫秒。 0 表示一直等待。

* @retval 返回一个指向DataBuffer的指针
*/
DataBuffer_ptr pop_buffer(uint32_t timeout_ms = 0);

/**
* @brief 将一个buffer推入queue中，实际入队的是指向该buffer结构的指针。
* @param [in] DataBuffer_ptr 指向DataBuffer的指针，或者nullptr
*/
void push_buffer(DataBuffer_ptr &buffer_ptr);

/**
* @brief 检查队列是否为空。
* @retval 为空返回true，否则返回false
*/
bool empty(void);

private:

std::queue<DataBuffer_ptr> _buffer_queue;
std::mutex _mutex;
std::condition_variable _cv;
}; // class BufferQueue

} // namespace logging

#endif // LOGGING_BUFFER_QUEUE_H_