#ifndef _LOGGING_LOG_STREAM_H_
#define _LOGGING_LOG_STREAM_H_

#include <functional>
#include <ostream>
#include <streambuf>
namespace logging {
/**
* @brief LogStream：Streaming buffer class. Inherit std::ostream to implement c++ streaming output, and inherit std::streambuf to implement buffer.
*/
class LogStream : public std::streambuf, public std::ostream {

   using int_type = typename std::streambuf::int_type;
   public:
    /**
 * @brief LogStream constructor
 * @param[in] buffer_size Output stream internal buffer size
 * @param[in] output_func Set the output interface. When the buffer is flushed, this interface will be called to implement data output.
 */
    LogStream(size_t buffer_size, std::function<void(const char *, size_t)> output_func);

    /**
* @note :The internal buffer needs to be released during destruction
*/
    ~LogStream(void);

    /**
* @brief The sputc() and sputn() call this function in case of an overflow (pptr() == nullptr or pptr() >= epptr()).
* @param [int_type] c : the character to store in the buffer
*/
    virtual int overflow(int_type c);

    /**
* @brief Flush the buffer and output the data in the buffer to a file or device
*/
    void flush_data(void);

    /**
* @brief reset internal buffer
*/
    void reset_buffer(void);

   private:
    char *_buffer;
    size_t _size;
    std::function<void(const char *, size_t)> _output_func;

};  // class LogStream

}  // namespace logging

#endif  // _LOGGING_LOG_STREAM_H_