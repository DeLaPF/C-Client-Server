#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

//https://github.com/progschj/ThreadPool

namespace dlpf::async {
    class thread_pool {
        public:
            thread_pool(size_t);
            template<class F, class... Args>
            auto enqueue(F&& f, Args&&... args) 
                -> std::future<typename std::result_of<F(Args...)>::type>;
            ~thread_pool();
        private:
            // need to keep track of threads so we can join them
            std::vector< std::thread > workers;
            // the task queue
            std::queue< std::function<void()> > tasks;

            // synchronization
            std::mutex queue_mutex;
            std::condition_variable condition;
            bool stop;
    };
} // dlpf::async

#endif
