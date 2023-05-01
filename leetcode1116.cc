#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>

using std::function;
using namespace std::chrono_literals;


void printNumber(int a) {
    std::cout << a << std::endl;
}

class ZeroEvenOdd {
private:
    int n;
    int iteration = 1;
    std::mutex mtx;
    std::condition_variable cv;


public:
    ZeroEvenOdd(int n) {
        this->n = n;
        std::cout << "aa" << std::endl;
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
    while(this->iteration < 2*this->n) {
       std::cout << "iteration num" << this->iteration << "thread " << "zero" << std::endl;
    std::unique_lock lk(mtx);
    cv.wait(lk, [this]{
        if(this->iteration % 2 != 0){
        return true;
        } else {
        return false;
        }
        });
    std::cout << "bb" << std::endl;
    printNumber(0);
    this->iteration++;
    lk.unlock();
    cv.notify_all();
    }
    }

    void even(function<void(int)> printNumber) {
    while(this->iteration < 2*this->n) {
        std::cout << "iteration num" << this->iteration << "thread " << "even" << std::endl;
    std::unique_lock lk(mtx);
    cv.wait_until(lk,std::chrono::system_clock::now() + 10ms, [this]{
        if(this->iteration % 2 == 0 && this->iteration % 4 == 0) {
        return true;
        } else {
        return false;
        }
        });
    std::cout << "cc" << std::endl;
    printNumber(this->iteration/2);
    this->iteration++;
    lk.unlock();
    cv.notify_all();
    }
    }

    void odd(function<void(int)> printNumber) {
    while(this->iteration < 2*this->n) {
        std::cout << "iteration num" << this->iteration << "thread " << "odd" << std::endl;
    std::unique_lock lk(mtx);
    cv.wait_until(lk,std::chrono::system_clock::now() + 10ms, [this]{
        if(this->iteration % 2 == 0 && this->iteration % 4 != 0) {
        return true;
        } else {
        return false;
        }
        });
    std::cout << "dd" << std::endl;
    printNumber(this->iteration/2);
    this->iteration++;
    lk.unlock();
    cv.notify_all();
    }
    }
};


int main() {
    ZeroEvenOdd object(2);
    std::thread t1(&ZeroEvenOdd::zero, &object, printNumber);
    std::thread t2(&ZeroEvenOdd::even, &object, printNumber);
    std::thread t3(&ZeroEvenOdd::odd, &object, printNumber);
    t1.join();
    std::cout << "t1 finished!!" << std::endl;
    t2.join();
    t3.join();
    std::cout << "finished!!" << std::endl;
    return 0;
}