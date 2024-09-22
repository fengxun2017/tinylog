// #include <gperftools/profiler.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "logging.h"

using namespace logging;

// void
// write_fun (void)
// {
//     for (uint32_t i = 0; i < (1000000); i++)
//     {
//         logger.append_data(data, size);
//         if(i%100==0)
//             std::this_thread::sleep_for(std::chrono::microseconds(5));
//     }
// }

int
main (void)
{
    std::cout << "start main\n";
    LOG(DEBUG) << "test debug log" << std::endl;
    LOG(INFO) << "test INFO log" << std::endl;
    LOG(WARNING) << "test WARNING log" << std::endl;
    LOG(ERROR) << "test ERROR log" << std::endl;
    // ProfilerStop();
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