#include "async_logging.h"
#include <chrono>
#include <cstring>
#include <iostream>
#include <new>

namespace logging {

/**
 * @brief Asynchronous logger initialization
 * @param [in] file_name: Log file name
 * @param [in] roll_cycle_minutes: Log file rolling period, in minutes.
 * @param [in] roll_size_bytes: File rolling size, in bytes
 * @note  If roll_cycle_minutes is equal to 0, no new log files will be
 * generated based on time rolling. If roll_size_bytes is equal to 0, new log
 * files will not be rolled based on the log file size.
 */
// FIXME: This work should go into the constructor
void
AsyncLogging::init(std::string file_name, uint64_t roll_cycle_minutes, uint64_t roll_size_bytes)
{

    _log_file_ptr.reset(new (std::nothrow) LogFile(file_name, roll_cycle_minutes, roll_size_bytes));
    if (nullptr == _log_file_ptr)
    {
        std::cerr << "[AsyncLogging::init] can not create file !!!!!\n";
        return;
    }
    /* In the initial state, a total of 11 free buffers are available, and the
     * buffer with data is 0 */
    _input_queue_ptr
        = std::unique_ptr<BufferQueue>(new (std::nothrow) BufferQueue(NUM_OF_AVAILABLE_BUFFERS));
    _output_queue_ptr = std::unique_ptr<BufferQueue>(new (std::nothrow) BufferQueue(0));
    _cur_buffer_ptr   = std::unique_ptr<DataBuffer>(new (std::nothrow) DataBuffer());

    if ((nullptr == _input_queue_ptr) || (nullptr == _output_queue_ptr)
        || (nullptr == _cur_buffer_ptr))
    {
        std::cerr << "[AsyncLogging::init] can not create buffer queue !!!!!\n";
    }
}

/**
 * @brief Logger destructor
 * @note When destructing, you need to check whether there is still data in the
 * buffer, and if so, refresh the buffer.
 */
AsyncLogging::~AsyncLogging(void)
{

    _running = false;

    if (nullptr != _log_file_ptr)
    {
        /* Clear the cached data in the output queue */
        while (!_output_queue_ptr->empty())
        {
            auto tmp = _output_queue_ptr->pop_buffer(1);
            if (nullptr != tmp)
            {
                _log_file_ptr->write_logdata(tmp->get_buffer(), tmp->get_data_size());
                tmp->reset_buffer();
            }
        }
        /* The currently held buffer may also have data that has not been
         * written. */
        std::unique_lock<std::mutex> lock(_buffer_lock);
        if(nullptr != _cur_buffer_ptr)
        {
            size_t                      data_size = _cur_buffer_ptr->get_data_size();
            if (data_size > 0)
            {
                _log_file_ptr->write_logdata(_cur_buffer_ptr->get_buffer(), data_size);
                _cur_buffer_ptr->reset_buffer();
            }
        }
        lock.unlock();

        _log_file_ptr->flush();
    }

    if (_background_thread.joinable())
    {
        _background_thread.join();
    }
}

/**
 * @brief Log data writing
 * @param [in] data : Log data source address
 * @param [in] size : Log data length
 */
void
AsyncLogging::append_data(const char *data, size_t size)
{

    std::unique_lock<std::mutex> lock(_buffer_lock);
    if (nullptr != _cur_buffer_ptr)
    {
        size_t                       data_size = _cur_buffer_ptr->get_data_size();
        if (data_size + size > _cur_buffer_ptr->get_buffer_size())
        {
            _output_queue_ptr->push_buffer(_cur_buffer_ptr);

            /* !!! FIXME: The caller should not be blocked due to logging
             * issues*/
            _cur_buffer_ptr = _input_queue_ptr->pop_buffer(1);
            if (nullptr == _cur_buffer_ptr)
            {
                std::cerr << "\n!!!Log input is too fast!!!\n";
                return;
            }
        }
        _cur_buffer_ptr->input_data(data, size);
        lock.unlock();
    }
    else
    {
        lock.unlock();
        std::cerr << "[AsyncLogging::append_data] _cur_buffer_ptr is null" << std::endl;
    }
}

/**
 * @brief Background log consumption thread implementation, responsible for
 * writing log data into log files
 */
void
AsyncLogging::background_consume_thread(void)
{

    while (_running)
    {
        if (nullptr != _log_file_ptr)
        {
            DataBuffer_ptr buffer_ptr = _output_queue_ptr->pop_buffer(1000);
            if (nullptr != buffer_ptr)
            {
                _log_file_ptr->write_logdata(buffer_ptr->get_buffer(), buffer_ptr->get_data_size());
                buffer_ptr->reset_buffer();
                {
                    std::lock_guard<std::mutex> lock(_buffer_lock);
                    if(nullptr == _cur_buffer_ptr)
                    {
                        _cur_buffer_ptr = std::move(buffer_ptr);
                        std::cout << "[AsyncLogging::background_consume_thread] reset _cur_buffer_ptr" << std::endl;
                    }
                }

                if(nullptr != buffer_ptr)
                {
                    _input_queue_ptr->push_buffer(buffer_ptr);
                }

            }
            else
            {
                /* If there is no buffer in the output queue, write the data in
                 * the buffer pointed to by _cur_buffer_ptr to the log file. */
                DataBuffer_ptr tmp = nullptr;
                {
                    std::lock_guard<std::mutex> lock(_buffer_lock);
                    if (nullptr != _cur_buffer_ptr)
                    {
                        tmp             = std::move(_cur_buffer_ptr);
                        _cur_buffer_ptr = _input_queue_ptr->pop_buffer(1);
                    }
                }
                if (nullptr == _cur_buffer_ptr)
                {
                    std::cerr << "[AsyncLogging::background_consume_thread] cant not get free buffer" << std::endl;
                }

                if (nullptr != tmp && tmp->get_data_size() > 0)
                {
                    _log_file_ptr->write_logdata(tmp->get_buffer(), tmp->get_data_size(), true);
                    tmp->reset_buffer();
                    _input_queue_ptr->push_buffer(tmp);
                }
            }

            

        }
        else
        {
            std::cerr << "[AsyncLogging::background_consume_thread] file is null" << std::endl;
        }
    }
}

/**
 * @brief Start logging
 */
void
AsyncLogging::start()
{
    _running = true;
    std::thread t(&AsyncLogging::background_consume_thread, this);
    _background_thread = std::move(t);

    /* Wait for the thread to actually start */
    while (!_background_thread.joinable())
    {
    }
}

} // namespace logging
