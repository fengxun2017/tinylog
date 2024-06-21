#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include "async_logging.h"
#include <gperftools/profiler.h>

using namespace logging;

AsyncLogging logger;
const char * data = "this DEBUG test this INFO test this WARNING test this FATAL test\n";
int size = strlen(data);

void write_fun(void) {
for (uint32_t i = 0; i < (500000); i++) {
logger.append_data(data, size);
}
}

int main(void) {
//ProfilerStart("test_capture.prof");
logger.init("test_time_cycle.log", 10, 0);
std::cout << "start main\n";
std::chrono::time_point<std::chrono::system_clock> end;
std::chrono::time_point<std::chrono::system_clock> start;
logger.start();
start = std::chrono::system_clock::now();
std::vector<std::thread> threads;
for (int i = 0; i < 10; i++) {
threads.push_back(std::thread(write_fun));
}
for (auto &t : threads) {
t.join();
}
end = std::chrono::system_clock::now();
std::chrono::duration<float> cost = end-start ;
std::cout << "consume time:" << cost.count() << std::endl;

std::cout << "main end!\n";
//ProfilerStop();
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