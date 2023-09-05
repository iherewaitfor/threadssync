// condition_variable::notify_one
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include<Windows.h>
#include <vector>

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
        if (g_stop) {
            break;
        }
        std::unique_lock<std::mutex> lck(mtx);
        while (carCount == 0) {
            CAR_NOT_ZERO.wait(lck);//等待条件：汽车数据非0，才进行消费。wait阻塞时，释放lock。返回时取得lock
            if (g_stop) {
                lck.unlock();
                std::cout << "consumer: " << std::this_thread::get_id() << "  exit" << '\n';
                return;
            }
        }
        bool needNotify = false;
        if (carCount == MAX_BUFFER_SIZE) {
            needNotify = true; //决定是否要通知生产者生产汽车。
        }
        carCount--; //消费汽车
        std::cout << "consumer: " << std::this_thread::get_id() << " carCount:" << carCount << '\n';
        lck.unlock(); //可先释放锁，否则被唤醒的线程因抢不锁又被被锁住了。
        if (needNotify) {
            CAR_NOT_MAX.notify_one();
        }
        //若不显式调用unlock方法，unique_lock离开作用域自动解锁
    }
    std::cout << "consumer: " << std::this_thread::get_id() << "  exit" << '\n';
}

void producer() {
    while (!g_stop) {
        Sleep(200);
        if (g_stop) {//醒来后先看看是不是要退出了。
            break;
        }
        std::unique_lock<std::mutex> lck(mtx);
        while (carCount == MAX_BUFFER_SIZE) {
            CAR_NOT_MAX.wait(lck);
            if (g_stop) {
                lck.unlock();
                std::cout << "producer: " << std::this_thread::get_id() << "  exit" << '\n';
                return;
            }
        }
        bool needNotify = false;
        if (carCount == 0) {
            needNotify = true; //决定是否要通知消费者消费汽车。原来没有，现在有了。
        }
        carCount++;//生产汽车
        std::cout << "producer: " << std::this_thread::get_id() << " carCount:" << carCount << '\n';
        lck.unlock(); //可先释放锁，否则被唤醒的线程因抢不锁又被被锁住了。
        if (needNotify) {
            CAR_NOT_ZERO.notify_one();
        }
        //若不显式调用unlock方法，unique_lock离开作用域自动解锁
    }
    std::cout << "producer: " << std::this_thread::get_id() << "  exit" << '\n';
}

int main()
{
    std::vector<std::thread> consumers; //消费线程
    std::vector<std::thread> producers; //生产线程

    for (int i = 0; i < 5; ++i) {
        consumers.push_back(std::thread(consumer));
    }
    for (int i = 0; i < 2; ++i) {
        producers.push_back(std::thread(producer));
    }

    std::cout << "press any key and enter to stop" << std::endl;
    char c = getchar();
    g_stop = true;
    CAR_NOT_ZERO.notify_all();//叫醒所有等待该条件的线程，退出。注意sleep中的线程没有在等待该条件，不会收到通知
    CAR_NOT_MAX.notify_all(); //叫醒所有等待该条件的线程，退出。
    for (auto& t : consumers) {
        if (t.joinable()) {//没有调用过join或detach，都应该能join
            t.join();
        }
    }
    for (auto& t : producers) {
        if (t.joinable()) {
            t.join();
        }
    }
    //waitfor the  worker threads exit
    std::cout << "all workder threads exit" << std::endl;
    return 0;
}