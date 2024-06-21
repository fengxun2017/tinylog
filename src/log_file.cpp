#include <string>
#include <iostream>
#include <chrono>
#include <time.h> // strftime localtime_r
#include "log_file.h"

namespace logging {

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
LogFile::LogFile(std::string file_name, uint64_t roll_cycle_minutes, uint64_t roll_size_bytes)
:_roll_size_bytes(roll_size_bytes), _roll_cycle_minutes(roll_cycle_minutes),
_file_name(file_name) {

_file_create_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
_output_file.reset(new BaseFile(_file_name));
}

/**
* @brief 滚动当前日志文件，将当前日志文件保存为_file_name.YMDH的形式
*/
void LogFile::roll_log_file(void) {
char new_file_name[100] = {0};

// 这里不使用localtime,linux中localtime存储转换结果的数据区域会和其它函数共享(如gmtime)，所以结果可能会被其它函数调用结果覆盖。
std::tm tm_data;
localtime_r(&_file_create_time, &tm_data);

// close and rename
std::string file_name_format = _file_name + ".%Y%m%d%H%M";
std::strftime(new_file_name, sizeof(new_file_name), file_name_format.c_str(), &tm_data);
_output_file->close();
_output_file->rename(_file_name.c_str(), new_file_name);

_output_file.reset(new BaseFile(_file_name));
_file_create_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

/**
* @brief 写入日志数据
* @param [in] data : 待写入数据的buffer地址
* @param [in] size : 待写入数据的字节数
* @param [in] flush_now : 是否立刻将数据刷新到文件中
*/
void LogFile::append_data(const char *data, uint32_t size, bool flush_now) {

_output_file->append_data(data, size, flush_now);

size_t written_bytes;
written_bytes = _output_file->get_written_bytes();

std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
uint64_t create_minute = _file_create_time/SECONDS_PER_MINUTE;
uint64_t now_minute = now/SECONDS_PER_MINUTE;

if ((0 != _roll_size_bytes) && (written_bytes >= _roll_size_bytes)) {
roll_log_file();
return ;
} 
if ((0 != _roll_cycle_minutes) && ((now_minute-create_minute) >= _roll_cycle_minutes)) {
roll_log_file();
}
}

/**
* @brief 缓冲区数据清空
*/
void LogFile::flush(void) {
_output_file->flush();
}

} // namespace logging