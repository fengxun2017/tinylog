#ifndef _LOGGING_APPEND_FILE_H_
#define _LOGGING_APPEND_FILE_H_

#include <string>

namespace logging {

/**
* @brief 附加写文件类。
* @note 线程不安全类
*/
class BaseFile {
public:
BaseFile(const std::string file_name);
~BaseFile(void);

void append_data(const char *data, size_t size, bool fulsh_now = false);
size_t get_written_bytes(void);
void rename(const char *old_filename, const char *new_filename);
void close(void);
void flush(void);

private:
char _local_buffer[64*1024];
FILE * _file;
size_t _written_bytes;

}; // class BaseFile

} // namespace logging

#endif // LOGGING_APPEND_FILE_H_