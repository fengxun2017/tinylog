#include "log_stream.h"
#include <iostream>
#include <streambuf>
#include "fast_memcpy.h"

namespace logging {
/**
 * @brief LogStream constructor
 * @param[in] buffer_size Output stream internal buffer size
 * @param[in] output_func Set the output interface. When the buffer is flushed,
 * this interface will be called to implement data output.
 */
LogStream::LogStream(size_t buffer_size, std::function<void(const char *, size_t)> output_func)
    : std::ostream(this)
{
    // 设置 streambuf
    _buffer = new (std::nothrow) char[buffer_size];
    if (nullptr == _buffer)
    {
        setp(nullptr, nullptr);
        _size = 0;
    }
    else
    {
        _size = buffer_size;
        setp(_buffer, _buffer + _size);
    }
    _output_func = output_func;
}

/**
 * @note :The internal buffer needs to be released during destruction
 */
LogStream::~LogStream(void)
{
    /* FIXME: Manage with smart pointer */
    delete[] _buffer;
    // reset pointer
    _buffer = nullptr;
    setp(nullptr, nullptr);
}

/**
 * @brief The sputc() and sputn() call this function in case of an overflow
 * (pptr() == nullptr or pptr() >= epptr()).
 * @param [in] c : the character to store in the buffer
 */
std::streambuf::int_type
LogStream::overflow(std::streambuf::int_type c)
{
    // std::cerr << "overflow" << std::endl;
    /* The currently used streambuf is not enough, expand the buffer */
    size_t new_size   = (_size + 1) * 3 / 2;
    char  *new_buffer = new (std::nothrow) char[new_size];
    if (nullptr == new_buffer)
    {
        delete[] _buffer;
        _buffer = nullptr;
        setp(NULL, NULL);
        std::cerr << "[LogStream::overflow] Failed to expand buffer" << std::endl;
        return std::streambuf::traits_type::eof();
    }
    /* Copy data to new buffer */
    memcpy_fast(new_buffer, _buffer, _size);
    delete[] _buffer;
    _buffer = new_buffer;

    /* Set the data range of the new buffer */
    setp(_buffer, _buffer + new_size);
    // reset put pointer
    pbump(_size);
    _size = new_size;

    /* The overflowed character is written to the new buffer. */
    return sputc(c);
}

/**
 * @brief Flush the buffer and output the data in the buffer to a file or device
 */
void
LogStream::flush_data(void)
{
    if (pbase() != pptr() && (nullptr != _output_func))
    {
        _output_func(pbase(), pptr() - pbase());
    }
}

/**
 * @brief reset internal buffer
 */
void
LogStream::reset_buffer(void)
{
    if (nullptr == _buffer)
    {
        setp(nullptr, nullptr);
        _size = 0;
    }
    else
    {
        setp(_buffer, _buffer + _size);
    }
}

} // namespace logging