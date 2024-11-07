// #include <gperftools/profiler.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "logging.h"

void
write_fun (std::string name)
{
    std::cout << "start " << name << std::endl;
    for (uint32_t i = 0; i < (5000); i++)
    {
        LOG(DEBUG) << "output to logfile:" << name << i << std::endl;
    }
}

int
main (void)
{
    logging::LogContorl cfg;
    LOG(DEBUG) << "Output to a standard terminal" << std::endl;
    LOG(INFO) << "Output to a standard terminal" << std::endl;
    LOG(WARNING) << "Output to a standard terminal" << std::endl;
    LOG(ERROR) << "Output to a standard terminal" << std::endl;

    cfg.use_ms = false;
    cfg.show_path = false;
    cfg.show_func = true;
    cfg.level = logging::LOG_DEBUG;
    cfg.logfile = "mylog.log";
    cfg.roll_cycle_minutes = 0;
    cfg.roll_size_bytes = 1024*1024;
    logging::log_init(cfg);

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++)
    {
        threads.push_back(std::thread(write_fun, std::to_string(i) + "task "));
    }

    for (int i = 0; i < 5; i++)
    {
        threads[i].join();
    }
    return 0;
}

// int main(void) {
// //ProfilerStart("test_capture.prof");

// logger.init("test_time_cycle.log", 10, 0);
// std::cout << "start main\n";
// std::chrono::time_point<std::chrono::system_clock> end;
// std::chrono::time_point<std::chrono::system_clock> start;
// logger.start();
// start = std::chrono::system_clock::now();
// for (uint32_t i = 0; i < (5000000); i++) {
// logger.append_data(data, size);
// }

// end = std::chrono::system_clock::now();
// std::chrono::duration<float> cost = end-start ;
// std::cout << "consume time:" << cost.count() << std::endl;

// std::cout << "main end!\n";
// //ProfilerStop();
// return 0;
// }