#include <iostream>
#include <thread>
#include "buffer_queue.h"

/**
 * @brief producer线程从input_queue中获取空闲 DataBuffer_ptr。
 * 写入数据后，将该DataBuffer_ptr再推入output_queue中。
 * consumer线程从ouput_queue中获取 DataBuffer_ptr,并提取其中数据。
 * 清空数据后，将该DataBuffer_ptr再推入input_queue中。
 */

bool thread1_done = false;
bool thread2_done = false;

void
producer_thread1 (logging::BufferQueue &input_queue, logging::BufferQueue &output_queue)
{
    std::cout << "start producer1\n";
    char val = 1;
    for (int i = 0; i < 50000; i++)
    {

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::cout << "thread1 get input buffer\n";
        logging::DataBuffer_ptr ptr = input_queue.pop_buffer();
        ptr->input_data(&val, 1);
        output_queue.push_buffer(ptr);
        std::cout << "thread1 push output buffer\n";
    }

    thread1_done = true;
}

void
producer_thread2 (logging::BufferQueue &input_queue, logging::BufferQueue &output_queue)
{
    std::cout << "start producer2\n";
    char val = 1;

    for (int i = 0; i < 50000; i++)
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        logging::DataBuffer_ptr ptr = input_queue.pop_buffer();
        std::cout << "thread2 get input buffer\n";
        ptr->input_data(&val, 1);
        output_queue.push_buffer(ptr);
        std::cout << "thread2 push output buffer\n";
    }
    thread2_done = true;
}

uint32_t global_val = 0;
void
consumer_thread (logging::BufferQueue &input_queue, logging::BufferQueue &output_queue)
{
    std::cout << "start consumer\n";
    while ((!thread1_done) || (!thread2_done))
    {

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        logging::DataBuffer_ptr ptr = output_queue.pop_buffer(1);
        if (nullptr != ptr)
        {
            for (int j = 0; j < ptr->get_data_size(); j++)
            {
                global_val += ptr->get_buffer()[0];
            }
            std::cout << "global_val:" << global_val << std::endl;
            ptr->reset_buffer();
            input_queue.push_buffer(ptr);
        }
    }
    while (1)
    {
        logging::DataBuffer_ptr ptr = output_queue.pop_buffer(1);
        if (nullptr != ptr)
        {
            for (int j = 0; j < ptr->get_data_size(); j++)
            {
                global_val += ptr->get_buffer()[0];
            }
            std::cout << "global_val:" << global_val << std::endl;
            ptr->reset_buffer();
            input_queue.push_buffer(ptr);
        }
        else
        {
            return;
        }
    }
}

int
main (void)
{

    logging::BufferQueue input_queue(10), output_queue(0);
    std::thread          p1(producer_thread1, std::ref(input_queue), std::ref(output_queue));
    std::thread          p2(producer_thread2, std::ref(input_queue), std::ref(output_queue));
    std::thread          c1(consumer_thread, std::ref(input_queue), std::ref(output_queue));

    p1.join();
    p2.join();
    c1.join();

    std::cout << "result:" << global_val << std::endl;

    return 0;
}