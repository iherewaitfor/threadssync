// condition_variable::notify_one
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include<Windows.h>

std::mutex mtx;
std::condition_variable CAR_NOT_MAX;  //to wakeup product
std::condition_variable CAR_NOT_ZERO; //to wakeup consumer

//汽车数量
int carCount = 0;     // shared value by producers and consumers
const int MAX_BUFFER_SIZE = 3;
volatile bool g_stop = false;

void consumer() {
    while (!g_stop) {
        Sleep(1000);
        std::unique_lock<std::mutex> lck(mtx);
        while (carCount == 0) {
            CAR_NOT_ZERO.wait(lck);//等待条件：汽车数据非0，才进行消费
        }
        bool needNotify = false;
        if (carCount == MAX_BUFFER_SIZE) {
            needNotify = true; //决定是否要通知生产者生产汽车。
        }
        carCount--; //消费汽车
        std::cout << "consumer: " << std::this_thread::get_id() << " carCount:" << carCount << '\n';
        if (needNotify) {
            CAR_NOT_MAX.notify_one();
        }
    }
}

void producer() {
    while (!g_stop) {
        Sleep(200);
        std::unique_lock<std::mutex> lck(mtx);
        while (carCount == MAX_BUFFER_SIZE) {
            CAR_NOT_MAX.wait(lck);
        }
        bool needNotify = false;
        if (carCount == 0) {
            needNotify = true; //决定是否要通知生产者生产汽车。
        }
        carCount++;//生产汽车
        std::cout << "producer: " << std::this_thread::get_id() << " carCount:" << carCount << '\n';
        if (needNotify) {
            CAR_NOT_ZERO.notify_one();
        }
    }
}

int main()
{
    std::thread consumers[3]; //消费线程
    std::thread producers[2]; //生产线程

    for (int i = 0; i < sizeof(consumers)/sizeof(std::thread); ++i) {
        consumers[i] = std::thread(consumer);
    }
    for (int i = 0; i < _countof(consumers); ++i) {
        consumers[i].detach();
    }

    for (int i = 0; i < _countof(producers); ++i) {
        producers[i] = std::thread(producer);
    }
    for (int i = 0; i < _countof(producers); ++i) {
        producers[i].detach();
    }
    std::cout << "press any key and enter to stop" << std::endl;
    char c = getchar();
    g_stop = true;
    //waitfor the  worker threads exit
    std::cout << "press any key and enter to exit" << std::endl;
    std::cin>>c;
    return 0;
}