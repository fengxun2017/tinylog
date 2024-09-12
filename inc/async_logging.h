#ifndef _LOGGING_ASYNC_LOGGING_H_
#define _LOGGING_ASYNC_LOGGING_H_

#include <memory>
#include <mutex>
#include <thread>
#include "buffer_queue.h"
#include "log_file.h"

namespace logging {
/**
* @brief Asynchronous log class supports multi-threaded log writing.
* It is actually composed of two queues internally, the input-oriented input_queue, and the output-oriented output_queue.
* When writing the log, obtain the free buffer_ptr from the input_queue, write the log into the buffer, and then put the buffer into the output_queue.
* The background daemon task (only one) obtains the buffer_ptr from the output_queue, writes the data in the buffer to the file, and then puts the buffer_ptr back into the input_queue for next writing.
*/

class AsyncLogging {

   public:
    /**
* @brief Asynchronous logger initialization
* @param [in] file_name: Log file name
* @param [in] roll_cycle_minutes: Log file rolling period, in minutes.
* @param [in] roll_size_bytes: File rolling size, in bytes
* @note  If roll_cycle_minutes is equal to 0, no new log files will be generated based on time rolling.
*        If roll_size_bytes is equal to 0, new log files will not be rolled based on the log file size.
*/
    void init(std::string file_name, uint64_t roll_cycle_minutes = 0,
              uint64_t roll_size_bytes = 0);

    /**
* @brief Logger destructor
* @note When destructing, you need to check whether there is still data in the buffer, and if so, refresh the buffer.
*/
    ~AsyncLogging(void);

    AsyncLogging(void) : _cur_buffer_ptr(nullptr), _running(false) {}

    /**
* @brief Log data writing
* @param [in] data : Log data source address
* @param [in] size : Log data length
*/
    void append_data(const char *data, size_t size);

    /**
* @brief start background daemon task
*/
    void start();


   private:
    static const size_t NUM_OF_AVAILABLE_BUFFERS = 10;   //Each buffer size is 1M
    
    /**
* @brief Background log consumption thread implementation, responsible for writing log data into log files
*/
    void background_consume_thread(void);

    /* The buffer currently in use */
    DataBuffer_ptr _cur_buffer_ptr;

    /* Mutex lock to ensure thread safety of access to _cur_buffer_ptr */
    std::mutex _buffer_lock;

    /* Points to the input queue, from which the logger obtains the free buffer, fills it with log data and then puts it into the output queue. */
    std::unique_ptr<BufferQueue> _input_queue_ptr;

    /* Points to the output queue, from which the logger obtains a buffer with data, writes the data to the log file, and then puts the empty buffer back into the input queue. */
    std::unique_ptr<BufferQueue> _output_queue_ptr;

    /* background daemon task running status */
    bool _running;
    
    std::thread _background_thread;
    std::unique_ptr<LogFile> _log_file_ptr;
};

}  // namespace logging
#endif  // _LOGGING_ASYNC_LOGGING_H_