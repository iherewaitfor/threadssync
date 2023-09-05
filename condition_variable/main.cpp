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

//��������
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
            CAR_NOT_ZERO.wait(lck);//�ȴ��������������ݷ�0���Ž������ѡ�wait����ʱ���ͷ�lock������ʱȡ��lock
            if (g_stop) {
                lck.unlock();
                std::cout << "consumer: " << std::this_thread::get_id() << "  exit" << '\n';
                return;
            }
        }
        bool needNotify = false;
        if (carCount == MAX_BUFFER_SIZE) {
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
        if (g_stop) {//�������ȿ����ǲ���Ҫ�˳��ˡ�
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
    std::vector<std::thread> consumers; //�����߳�
    std::vector<std::thread> producers; //�����߳�

    for (int i = 0; i < 5; ++i) {
        consumers.push_back(std::thread(consumer));
    }
    for (int i = 0; i < 2; ++i) {
        producers.push_back(std::thread(producer));
    }

    std::cout << "press any key and enter to stop" << std::endl;
    char c = getchar();
    g_stop = true;
    CAR_NOT_ZERO.notify_all();//�������еȴ����������̣߳��˳���ע��sleep�е��߳�û���ڵȴ��������������յ�֪ͨ
    CAR_NOT_MAX.notify_all(); //�������еȴ����������̣߳��˳���
    for (auto& t : consumers) {
        if (t.joinable()) {//û�е��ù�join��detach����Ӧ����join
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