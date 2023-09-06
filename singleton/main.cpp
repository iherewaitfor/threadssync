#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <vector>

//懒汉模式，按需创建实例。
class Singleton {
private:
    static std::mutex mut;
    static Singleton* instence;
    Singleton() {}
    Singleton(const Singleton& tmp) {}
    Singleton& operator=(const Singleton& tmp) {}
public:
    static Singleton* getInstence() {
        if (instence == NULL) {
            std::lock_guard<std::mutex> scopLock(mut);
            if (instence == NULL) {
                std::cout << " Singleton inited by" << std::this_thread::get_id() << std::endl;
                instence = new Singleton();
            }
        }
        return instence;
    }
    void pintHi() {
        std::cout << " hi from Singleton in thread: " << std::this_thread::get_id() << std::endl;
    }
};
//静态成员变量，类外初始化
Singleton* Singleton::instence = NULL;
std::mutex Singleton::mut;

//饿汉模式，提前创建实例。静态成员初始化在main函数运行之前。
class MySingle {
private:
    static MySingle* instence;
    MySingle(){
        std::cout << " create MySingle in thread: " << std::this_thread::get_id() << std::endl;
    }
    MySingle(const MySingle& tmp) {}
public:
    static MySingle* getInstance() {
        return instence;
    }
    void pintHi() {
        std::cout << " hi from MySingle in thread: " << std::this_thread::get_id() << std::endl;
    }
};
//静态成员初始化在main函数运行之前。
MySingle * MySingle::instence = new MySingle();

void threadFun() {
    std::cout << "start thread:" << std::this_thread::get_id() << std::endl;
    Singleton::getInstence()->pintHi();
    MySingle::getInstance()->pintHi();
}

int main()
{
    std::cout << "main thread:" << std::this_thread::get_id() << std::endl;
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; i++) {
        threads.push_back(std::thread(threadFun));
    }
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}