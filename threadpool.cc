#include <iostream>
#include <thread>
#include <functional>
#include <queue>
#include <vector>
#include <condition_variable>

class ThreadPool{
    public:
        explicit ThreadPool(int num_threads):
        m_num_threads{num_threads} {
            start();
        }

        ~ThreadPool() {
            std::cout << "destructor called." << std::endl;
            stop();
        }

        void push(std::function<void()> func) {
            {
            std::unique_lock<std::mutex> lk(cv_m);
            m_queue.emplace(func);
            }
            cv.notify_one();
        }

    private:
        int m_num_threads;
        std::condition_variable cv;
        std::mutex cv_m;
        bool mStopping = false;
        std::queue<std::function<void()>> m_queue;
        std::vector<std::thread> m_workers;
        void start() {
            for(int i=0; i < m_num_threads; i++) {
                m_workers.emplace_back([=]{
            while(true) {
                std::function<void()> task;
                {
                std::unique_lock<std::mutex> lk(cv_m);
                cv.wait(lk, [this]{return mStopping || !m_queue.empty();});
                if(mStopping && m_queue.empty()) break;
                task = m_queue.front();
                m_queue.pop();
                }
                cv.notify_one();
                task();
            }
                });
            }
        }

        void stop() {
            mStopping = true;
            for(auto &thread: m_workers) {
                thread.join();
            }
        }
};


int main() {
    ThreadPool threadpool(2);
    threadpool.push([](){
        std::cout << "Hello World" << std::endl; });
    std::cout << "program over." << std::endl;
}