#ifndef _LOGGING_LOG_FILE_H_
#define _LOGGING_LOG_FILE_H_

#include <chrono>
#include <memory>
#include <string>
#include "base_file.h"

namespace logging {

/**
 * @brief Log file class, uses BaseFile to implement file writing
 * Whenever the file existence time reaches the set rolling cycle time, or the
 * file size reaches the set rolling size, a new file will be automatically
 * generated.
 */
class LogFile
{
public:
    /**
     * @brief LogFile constructor
     * @param[in] file_name Log file name
     * @param[in] roll_cycle_minutes File rolling period, in minutes. If the
     * value is 0, no new files will be generated based on time rolling.
     * @param[in] roll_size_bytes File rolling size, in bytes. If the value is
     * 0, new log files will not be rolled based on the log file size.
     */
    LogFile(std::string file_name, uint64_t roll_cycle_minutes = 0, uint64_t roll_size_bytes = 0);

    /**
     * @brief Write log data
     * @param [in] logdata The source address of the data to be written
     * @param [in] size The size of the data to be written
     * @param [in] flush_now Whether to flush the buffer data to the file
     * immediately
     */
    void write_logdata(const char *logdata, uint32_t size, bool flush_now = false);

    /**
     * @brief Flush buffer data to file
     */
    void flush(void);

private:
    /**
     * @brief The current log file is saved in the format of logfile.YMDH. A new
     * log file is also generated.
     */
    void roll_log_file(void);

    std::string _file_name;

    /* Every how many minutes a new log file is generated. */
    uint64_t _roll_cycle_minutes;

    /* When the amount of data saved in the current log file is greater than
     * this value, a new log file is generated. */
    uint64_t _roll_size_bytes;

    /* Current log file */
    std::unique_ptr<BaseFile> _log_file;
    /* Current log file creation time */
    std::time_t _file_create_time;

    static const uint32_t SECONDS_PER_MINUTE = 60;
    static const uint32_t CHECK_PERIOD       = 1024;
    static const uint32_t MAX_FILENAME_SIZE  = 100;
}; // LogFile

} // namespace logging

#endif // LOGGING_LOG_FILE_H_