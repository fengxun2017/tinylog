#include "base_file.h"
#include <stdio.h>  //fopen, rename
#include <string.h> // setvbuf
#include <cerrno>   // errno
#include <chrono>
#include <iostream>
#include <string>

namespace logging {

/* not thread safe*/
static const char *
error_to_str (int errnum)
{
    static char error_str[512];
    return strerror_r(errnum, error_str, sizeof(error_str));
}

/**
 * @brief Rename file
 * @param[in] old_filename Identifies the path to the file to be renamed
 * @param[in] new_filename new path to file
 */
void
BaseFile::rename(const char *old_filename, const char *new_filename)
{
    int ret = ::rename(old_filename, new_filename);
    if (0 != ret)
    {
        std::cerr << "[BaseFile::rename] failed in logging::BaseFile::rename\n";
    }
}

/**
 * @brief close file
 */
void
BaseFile::close(void)
{
    if (NULL != _file)
    {
        fflush(_file);
        fclose(_file);
        _file = NULL;
    }
}

/**
 * @brief BaseFile constructor
 * @param[in] file_name The file name that needs to be created。
 */
BaseFile::BaseFile(const std::string file_name)
{
    _written_bytes = 0;

    /* The "e" indicates that the O_CLOEXEC flag is applied on the file */
    /* FIXME: does not meet the C standard */
    _file = fopen(file_name.c_str(), "ae");
    if (NULL == _file)
    {
        std::cerr << "[BaseFile::BaseFile] failed in "
                     "logging::BaseFile::BaseFile(), fopen return NULL\n";
    }
    else
    {
        int ret = setvbuf(_file, _local_buffer, _IOFBF, sizeof(_local_buffer));
        if (0 != ret)
        {
            std::cerr << "[BaseFile::BaseFile] failed in "
                         "logging::BaseFile::BaseFile(), setvbuf "
                         "error info:"
                      << error_to_str(errno) << std::endl;
        }
    }
}

BaseFile::~BaseFile(void)
{
    if (NULL != _file)
    {
        fflush(_file);
        fclose(_file);
        _file = NULL;
    }
}

/**
 * @brief Append data to file
 * @param[in] data Data source address to be written
 * @param[in] size Size of data to be written
 * @param[in] fulsh_now Whether to flush buffer data to the file
 */
void
BaseFile::append_data(const char *data, size_t size, bool flush_now)
{
    size_t remainder = size;

    if (NULL != _file)
    {
        do
        {
            size_t n = fwrite(data, 1, remainder, _file);

            // records the number of bytes written to the file
            _written_bytes += n;

            if (n != remainder)
            {
                if (ferror(_file))
                {
                    std::cerr << "[BaseFile::append_data] failed in "
                                 "logging::BaseFile::append_data, error info:"
                              << error_to_str(errno) << std::endl;
                    clearerr(_file);
                    break;
                }
            }
            remainder -= n;
            if(0 != remainder)
            {
                std::cerr << "[BaseFile::append_data] not completed" << std::endl;
            }

        } while (0 != remainder);

        if (flush_now)
        {
            fflush(_file);
        }
    }
    else
    {
        std::cerr << "[BaseFile::append_data] failed in "
                     "logging::BaseFile::append_data, file is NULL."
                  << std::endl;
    }
}

/**
 * @brief Get the amount of data written to the current file
 * @return Number of bytes
 */
size_t
BaseFile::get_written_bytes(void)
{
    return _written_bytes;
}

/**
 * @brief Flush file buffer
 */
void
BaseFile::flush(void)
{
    if (NULL != _file)
    {
        fflush(_file);
    }
}

} // namespace logging