#include "buffer_queue.h"
#include <thread>
#include <iostream>

/**
* @brief producer线程从input_queue中获取空闲 DataBuffer_ptr。
* 写入数据后，将该DataBuffer_ptr再推入output_queue中。
* consumer线程从ouput_queue中获取 DataBuffer_ptr,并提取其中数据。
* 清空数据后，将该DataBuffer_ptr再推入input_queue中。
*/
void producer_thread1(logging::BufferQueue &input_queue, logging::BufferQueue &output_queue) {
std::cout << "start producer1\n";
for (int i = 0; i < 500; i++) {

std::this_thread::sleep_for(std::chrono::milliseconds(10));
std::cout << "thread1 get input buffer\n";
logging::DataBuffer_ptr ptr = input_queue.pop_buffer();
ptr->_buffer[0] = 1;
ptr->_cur_size = 1;
output_queue.push_buffer(ptr);
std::cout << "thread1 push output buffer\n";

}

}

void producer_thread2(logging::BufferQueue &input_queue, logging::BufferQueue &output_queue) {
std::cout << "start producer2\n";

for (int i = 0; i < 500; i++) {
std::this_thread::sleep_for(std::chrono::milliseconds(10));
logging::DataBuffer_ptr ptr = input_queue.pop_buffer();
std::cout << "thread2 get input buffer\n";
ptr->_buffer[0] = 1;
ptr->_cur_size = 1;
output_queue.push_buffer(ptr);
std::cout << "thread2 push output buffer\n";
}
}


uint32_t global_val = 0;
void consumer_thread(logging::BufferQueue &input_queue, logging::BufferQueue &output_queue) {
std::cout << "start consumer\n";
for (int i = 0; i < 1000; i++) {

std::this_thread::sleep_for(std::chrono::milliseconds(10));
logging::DataBuffer_ptr ptr = output_queue.pop_buffer();
for (int j = 0; j < ptr->_cur_size; j++){
global_val += ptr->_buffer[0];
}
std::cout << "global_val:" << global_val << std::endl;
ptr->_cur_size = 0;
input_queue.push_buffer(ptr);
}
}

int main(void) {

logging::BufferQueue input_queue(10),output_queue(0);
std::thread p1(producer_thread1, std::ref(input_queue), std::ref(output_queue));
std::thread p2(producer_thread2, std::ref(input_queue), std::ref(output_queue));
std::thread c1(consumer_thread, std::ref(input_queue), std::ref(output_queue));

p1.join();
p2.join();
c1.join();

std::cout << "result:" << global_val <<std::endl;

return 0;
}