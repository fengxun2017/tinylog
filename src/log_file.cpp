#include "log_file.h"
#include <time.h> // strftime localtime_r
#include <chrono>
#include <iostream>
#include <string>

namespace logging {

/**
 * @brief LogFile constructor
 * @param[in] file_name Log file name
 * @param[in] roll_cycle_minutes File rolling period, in minutes. If the value
 * is 0, no new files will be generated based on time rolling.
 * @param[in] roll_size_bytes File rolling size, in bytes. If the value is 0,
 * new log files will not be rolled based on the log file size.
 */
LogFile::LogFile(std::string file_name, uint64_t roll_cycle_minutes, uint64_t roll_size_bytes)
    : _roll_size_bytes(roll_size_bytes)
    , _roll_cycle_minutes(roll_cycle_minutes)
    , _file_name(file_name)
{

    _file_create_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    /* create log file*/
    _log_file.reset(new (std::nothrow) BaseFile(_file_name));
}

/**
 * @brief The current log file is saved in the format of logfile.YMDH. A new log
 * file is also generated.
 */
void
LogFile::roll_log_file(void)
{
    char new_file_name[MAX_FILENAME_SIZE] = { 0 };
    static uint32_t index = 0;

    if (nullptr != _log_file)
    {
        std::tm tm_data;
        /* localtime not safe */
        localtime_r(&_file_create_time, &tm_data);

        /* close and rename */
        std::string file_name_format = _file_name + ".%Y%m%d%H%M%S_" + std::to_string(index);
        index += 1;

        std::strftime(new_file_name, sizeof(new_file_name), file_name_format.c_str(), &tm_data);
        _log_file->flush();
        _log_file->close();
        _log_file->rename(_file_name.c_str(), new_file_name);
    }
    _log_file.reset(new (std::nothrow) BaseFile(_file_name));
    _file_create_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

/**
 * @brief Write log data
 * @param [in] logdata The source address of the data to be written
 * @param [in] size The size of the data to be written
 * @param [in] flush_now Whether to flush the buffer data to the file
 * immediately
 */
void
LogFile::write_logdata(const char *logdata, uint32_t size, bool flush_now)
{

    size_t written_bytes = 0;
    bool   need_roll     = false;

    if (nullptr != _log_file)
    {
        _log_file->append_data(logdata, size, flush_now);

        written_bytes = _log_file->get_written_bytes();

        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        uint64_t    create_minute = _file_create_time / SECONDS_PER_MINUTE;
        uint64_t    now_minute    = now / SECONDS_PER_MINUTE;

        if ((0 != _roll_size_bytes) && (written_bytes >= _roll_size_bytes))
        {
            need_roll = true;
        }
        if ((0 != _roll_cycle_minutes) && ((now_minute - create_minute) >= _roll_cycle_minutes))
        {
            need_roll = true;
        }

        if (need_roll)
        {
            roll_log_file();
        }
    }
    else
    {
        std::cerr << "[LogFile::write_logdata] file is NULL" << std::endl;
    }
}

/**
 * @brief Flush buffer data to file
 */
void
LogFile::flush(void)
{
    if (nullptr != _log_file)
    {
        _log_file->flush();
    }
}

} // namespace logging