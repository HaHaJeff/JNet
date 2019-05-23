#ifndef THREADPOOLFUTURE_H
#define THREADPOOLFUTURE_H

#include <thread>
#include <condition_variable>
#include <algorithm>
#include <functional>
#include <atomic>
#include <queue>
#include <mutex>
#include <future>

typedef std::function<void()> Task;

class ThreadPool {
public:
  explicit ThreadPool(std::size_t threads = std::max(4u, std::thread::hardware_concurrency() * 2));

  template <class Func, class ...Args>
  auto AddTask(Func&& f, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type>;

  ~ThreadPool();

private:
  std::vector<std::thread> workers_;
  std::size_t pool_size_;
  std::queue<std::function<void()>> tasks_;
  std::size_t max_queue_size_ {100000};
  bool stop_{false};

  //TODO: synchronize
  std::condition_variable condition_;
  std::mutex mutex_;
};

ThreadPool::ThreadPool(std::size_t threads) {
  for (size_t i = 0; i < threads; ++i) {
    workers_.emplace_back(
        [this]
        {
        while(true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->mutex_);
            condition_.wait(lock, [this]{return this->stop_ || !tasks_.empty();});

            if (stop_ && tasks_.empty()) {
              return;
            }
            else if (!this->tasks_.empty()) {
              task = std::move(this->tasks_.front());
              this->tasks_.pop();
            }
            else {
              continue;
            }

          }
          task();
        }
        }
        );
  }
}

template <class Func, class ...Args>
auto ThreadPool::AddTask(Func&& f, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type>
{
  using return_type = typename std::result_of<Func(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));

  auto result = task->get_future();

  std::unique_lock<std::mutex> lock(mutex_);

  if (tasks_.size() >= max_queue_size_) {
    condition_.wait(lock, [this](){
          return tasks_.size() < max_queue_size_ || stop_;
        });
  }

  tasks_.emplace([task](){
      (*task)();   //lambda std::function<void()>;
      });

  condition_.notify_one();
  return result;
}

ThreadPool::~ThreadPool() {

  {
    std::unique_lock<std::mutex> lock(this->mutex_);
    stop_ = true;
  }

  for(auto &worker: workers_) {
    worker.join();
  }
}

#endif
