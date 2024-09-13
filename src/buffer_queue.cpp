#include "buffer_queue.h"
#include <chrono>
#include <condition_variable>
#include <memory>
#include <new>
#include <utility>
#include "fast_memcpy.h"

namespace logging {
/**
 * @brief Save input data into internal buffer
 * @param[in] data Data source address
 * @param[in] size data size
 */
void
DataBuffer::input_data(const char *data, size_t size)
{
    size_t left_space = _BUFFER_SIZE - _cur_size;
    size_t copy_size  = (left_space > size) ? size : (left_space);
    memcpy_fast(_buffer + _cur_size, data, copy_size);

    _cur_size += copy_size;
}

/**
 * @brief reset buffer
 */
void
DataBuffer::reset_buffer(void)
{
    _cur_size = 0;
}

/**
 * @brief BufferQueue constructor
 * @param[in] size The number of elements that can be stored in the queue
 */
BufferQueue::BufferQueue(size_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        _buffer_queue.emplace(new (std::nothrow) DataBuffer());
    }
}

/**
 * @brief Get a buffer from the buffer queue
 * @param [in] timeout_ms : This value represents the wait time when there are
 * no buffers in the queue. A value of 0 means wait forever.
 * @retval a pointer to the data buffer
 */
DataBuffer_ptr
BufferQueue::pop_buffer(uint32_t timeout_ms)
{
    std::unique_lock<std::mutex> lk(_mutex);
    while (_buffer_queue.empty())
    {
        if (0 == timeout_ms)
        {
            _cv.wait(lk);
        }
        else
        {
            auto ret = _cv.wait_for(lk, std::chrono::milliseconds(timeout_ms));
            if (std::cv_status::timeout == ret)
            {
                return nullptr;
            }
        }
    }
    DataBuffer_ptr buffer = std::move(_buffer_queue.front());
    /* The element is still in the queue, but the content has been moved away */
    _buffer_queue.pop();

    lk.unlock();

    return buffer;
}

/**
 * @brief Push a buffer into the queue
 * @param [in] DataBuffer_ptr pointer to buffer
 */
void
BufferQueue::push_buffer(DataBuffer_ptr &buffer_ptr)
{
    if (nullptr != buffer_ptr)
    {
        {
            std::lock_guard<std::mutex> lk(_mutex);
            _buffer_queue.push(std::move(buffer_ptr));
        }
        _cv.notify_all();
    }
}

/**
 * @brief Check if the queue is empty.
 * @retval Returns true if empty, false otherwise
 */
bool
BufferQueue::empty(void)
{
    bool                        ret = false;
    std::lock_guard<std::mutex> lk(_mutex);
    ret = _buffer_queue.empty();

    return ret;
}

} // namespace logging