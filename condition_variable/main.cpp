// condition_variable::notify_one
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include<Windows.h>

std::mutex mtx;
std::condition_variable CAR_NOT_MAX;  //to wakeup product
std::condition_variable CAR_NOT_ZERO; //to wakeup consumer

//��������
int carCount = 0;     // shared value by producers and consumers
const int MAX_BUFFER_SIZE = 3;
volatile bool g_stop = false;

void consumer() {
    while (!g_stop) {
        Sleep(1000);
        std::unique_lock<std::mutex> lck(mtx);
        while (carCount == 0) {
            CAR_NOT_ZERO.wait(lck);//�ȴ��������������ݷ�0���Ž������ѡ�wait����ʱ���ͷ�lock������ʱȡ��lock
            if (g_stop) {
                lck.unlock();
                std::cout << "consumer: " << std::this_thread::get_id() << "  exit" << '\n';
                return;
            }
        }
        bool needNotify = false;
        if (carCount == 1) {
            needNotify = true; //�����Ƿ�Ҫ֪ͨ����������������
        }
        carCount--; //��������
        std::cout << "consumer: " << std::this_thread::get_id() << " carCount:" << carCount << '\n';
        lck.unlock(); //�����ͷ��������򱻻��ѵ��߳����������ֱ�����ס�ˡ�
        if (needNotify) {
            CAR_NOT_MAX.notify_one();
        }
        //������ʽ����unlock������unique_lock�뿪�������Զ�����
    }
    std::cout << "consumer: " << std::this_thread::get_id() << "  exit" << '\n';
}

void producer() {
    while (!g_stop) {
        Sleep(200);
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
            needNotify = true; //�����Ƿ�Ҫ֪ͨ����������������ԭ��û�У��������ˡ�
        }
        carCount++;//��������
        std::cout << "producer: " << std::this_thread::get_id() << " carCount:" << carCount << '\n';
        lck.unlock(); //�����ͷ��������򱻻��ѵ��߳����������ֱ�����ס�ˡ�
        if (needNotify) {
            CAR_NOT_ZERO.notify_one();
        }
        //������ʽ����unlock������unique_lock�뿪�������Զ�����
    }
    std::cout << "producer: " << std::this_thread::get_id() << "  exit" << '\n';
}

int main()
{
    std::thread consumers[3]; //�����߳�
    std::thread producers[2]; //�����߳�

    for (int i = 0; i < sizeof(consumers)/sizeof(std::thread); ++i) {
        consumers[i] = std::thread(consumer);
        consumers[i].detach();
    }

    for (int i = 0; i < _countof(producers); ++i) {
        producers[i] = std::thread(producer);
        producers[i].detach();
    }
    std::cout << "press any key and enter to stop" << std::endl;
    char c = getchar();
    g_stop = true;
    CAR_NOT_MAX.notify_all(); //�������еȴ����������̣߳��˳���
    CAR_NOT_ZERO.notify_all();//�������еȴ����������̣߳��˳���
    //waitfor the  worker threads exit
    std::cout << "press any key and enter to exit" << std::endl;
    std::cin>>c;
    return 0;
}