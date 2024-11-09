#ifndef _LOGGING_BUFFER_QUEUE_H_
#define _LOGGING_BUFFER_QUEUE_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace logging {

/**
 * @brief Buffer data structure.
 *        Stored in the data queue is a pointer to the data structure.
 */
class DataBuffer
{
public:
    DataBuffer(void)
        : _cur_size(0)
    {
    }

    size_t get_buffer_size (void)
    {
        return _BUFFER_SIZE;
    }

    size_t get_data_size (void)
    {
        return _cur_size;
    }

    const char *get_buffer (void)
    {
        return _buffer;
    }
    /**
     * @brief Save input data into internal buffer
     * @param[in] data Data source address
     * @param[in] size data size
     */
    void input_data(const char *data, size_t size);

    /**
     * @brief reset buffer
     */
    void reset_buffer(void);

private:
    static const size_t _BUFFER_SIZE = 32 * 1024;
    /* The amount of data currently cached */
    size_t _cur_size;
    /* The buffer where the data is actually stored */
    char _buffer[_BUFFER_SIZE];
};
using DataBuffer_ptr = std::unique_ptr<DataBuffer>;

/**
 * @brief Thread-safe DataBuffer queue.
 * @note The elements stored in the queue are pointer data pointing to the
 * DataBuffer type.
 */
class BufferQueue
{
public:
    /**
     * @brief BufferQueue constructor
     * @param[in] size The number of elements that can be stored in the queue
     */
    BufferQueue(size_t size = 10);

    /**
     * @brief Get a buffer from the buffer queue
     * @param [in] timeout_ms : This value represents the wait time when there
     * are no buffers in the queue. A value of 0 means wait forever.
     * @retval a pointer to the data buffer
     */
    DataBuffer_ptr pop_buffer(uint32_t timeout_ms = 0);

    /**
     * @brief Push a buffer into the queue
     * @param [in] DataBuffer_ptr pointer to buffer
     */
    void push_buffer(DataBuffer_ptr &buffer_ptr);

    /**
     * @brief Check if the queue is empty.
     * @retval Returns true if empty, false otherwise
     */
    bool empty(void);

private:
    std::queue<DataBuffer_ptr> _buffer_queue;
    std::mutex                 _mutex;
    std::condition_variable    _cv;
}; // class BufferQueue

} // namespace logging

#endif // LOGGING_BUFFER_QUEUE_H_