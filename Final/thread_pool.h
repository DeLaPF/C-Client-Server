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

    // add new work item to the pool
    template<class F, class... Args>
    auto dlpf::async::thread_pool::enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared< std::packaged_task<return_type()> >(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if(stop)
                throw std::runtime_error("enqueue on stopped thread_pool");

            tasks.emplace([task](){ (*task)(); });
        }
        condition.notify_one();
        return res;
    }
} // dlpf::async

#endif //THREAD_POOL_H
