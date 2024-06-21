#ifndef _LOGGING_LOG_FILE_H_
#define _LOGGING_LOG_FILE_H_

#include <chrono>
#include <string>
#include <memory>
#include "base_file.h"

namespace logging{

/**
* @brief 日志文件类，使用AppendFile实现文件写入。
* 每当文件存在时间达到设定的滚动周期时间，或文件大小达到设定的滚动大小，会自动生成新文件。
* @note 线程不安全类
*/
class LogFile {
public:

/**
* @brief 创建日志文件
* @param [in] file_name: 文件名
* @param [in] roll_cycle_hours: 文件滚动周期，分钟为单位。
等于0，不会根据时间进行滚动生成新日志文件。
大于0，每roll_cycle个小时会生成新的日志文件。
* @param [in] roll_size_bytes: 文件滚动大小，字节为单位
等于0，不会根据日志文件大小滚动生成新日志文件。
大于0，当前日志文件写入字节大于该值时，自动创建新日志文件。
*/
LogFile(std::string file_name, uint64_t roll_cycle_minutes=0, uint64_t roll_size_bytes=0); 

/**
* @brief 写入日志数据
* @param [in] data : 待写入数据的buffer地址
* @param [in] size : 待写入数据的字节数
* @param [in] flush_now : 是否立刻将数据刷新到文件中
*/
void append_data(const char *data, uint32_t size, bool flush_now = false);

/**
* @brief 缓冲区数据清空
*/
void flush(void);

private:

/**
* @brief 滚动当前日志文件，将当前日志文件保存为_file_name.YMDH的形式
*/
void roll_log_file(void);

std::string _file_name; 
uint64_t _roll_cycle_minutes; ///< 每隔多少分钟生成一个新日志文件
uint64_t _roll_size_bytes; ///< 当前日志文件保存的数据量大于该值时，生成新的日志文件

std::unique_ptr<BaseFile> _output_file; ///< 实际持有的文件
std::time_t _file_create_time; ///< 记录文件创建时间

static const uint32_t SECONDS_PER_MINUTE = 60;
static const uint32_t CHECK_PERIOD = 1024;
}; // LogFile

} // namespace logging

#endif // LOGGING_LOG_FILE_H_