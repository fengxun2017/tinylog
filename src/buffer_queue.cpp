#include <memory>
#include <new>
#include <utility>
#include <chrono>
#include <condition_variable>
#include "buffer_queue.h"

namespace logging {
/**
* @brief 创建Buffer队列
* @param [in] size : queue中存储的buffer个数,默认存储10个buffer
* @note queue中存储的是指向buffer的指针，即从Queue中获取空闲buffer时，返回的是buffer指针。
* buffer不需要使用了，放回Queue中时，也是push buffer指针。
*/
BufferQueue::BufferQueue(uint32_t size) {
for (uint32_t i = 0; i < size; i++) {
_buffer_queue.emplace(new DataBuffer());
}
}

/**
* @brief 从queue中获取 buffer_ptr。
* @param [in] timeout_ms : 等待超时，单位为毫秒。0 表一直等待
* 
* @retval 返回一个指向DataBuffer的指针，或者nullptr
*/
DataBuffer_ptr BufferQueue::pop_buffer(uint32_t timeout_ms){
std::unique_lock<std::mutex> lk(_mutex);
while (_buffer_queue.empty()) {
if (0 == timeout_ms)
{
_cv.wait(lk);
} else {
auto ret = _cv.wait_for(lk, std::chrono::milliseconds(timeout_ms));
if (std::cv_status::timeout == ret) {
return nullptr;
}
}
}
DataBuffer_ptr buffer = std::move(_buffer_queue.front());
_buffer_queue.pop();
lk.unlock();

return buffer;
}

/**
* @brief 将一个buffer推入queue中，实际入队的是指向该buffer结构的指针。
* @param [in] DataBuffer_ptr 指向DataBuffer的指针
*/
void BufferQueue::push_buffer(DataBuffer_ptr &buffer_ptr){
std::unique_lock<std::mutex> lk(_mutex);
_buffer_queue.push(std::move(buffer_ptr));
lk.unlock();

_cv.notify_all();
}

/**
* @brief 检查队列是否为空。
* @retval 为空返回true，否则返回false
*/
bool BufferQueue::empty(void) {
std::unique_lock<std::mutex> lk(_mutex);
return _buffer_queue.empty();
}

} // namespace logging