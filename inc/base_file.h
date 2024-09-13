#ifndef _LOGGING_APPEND_FILE_H_
#define _LOGGING_APPEND_FILE_H_

#include <string>

namespace logging {

/**
* @brief Basic file class, supports only append writing.
*/
class BaseFile {
   public:
    /**
 * @brief BaseFile constructor
 * @param[in] file_name The file name that needs to be created。
 */
    BaseFile(const std::string file_name);

    ~BaseFile(void);

    /**
 * @brief Append data to file
 * @param[in] data Data source address to be written
 * @param[in] size Size of data to be written
 * @param[in] fulsh_now Whether to flush buffer data to the file
 */
    void append_data(const char *data, size_t size, bool fulsh_now = false);

    /**
 * @brief Get the amount of data written to the current file
 * @return Number of bytes
 */
    size_t get_written_bytes(void);

    /**
 * @brief Rename file
 * @param[in] old_filename Identifies the path to the file to be renamed
 * @param[in] new_filename new path to file
 */
    void rename(const char *old_filename, const char *new_filename);

    /**
 * @brief close file
 */
    void close(void);

    /**
 * @brief Flush file buffer
 */
    void flush(void);

   private:
    char _local_buffer[64 * 1024];
    FILE *_file;
    size_t _written_bytes;

};  // class BaseFile

}  // namespace logging

#endif  // LOGGING_APPEND_FILE_H_