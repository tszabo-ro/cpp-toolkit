#include <cpp-toolkit/thread_pool.h>

ThreadPool::ThreadPool(unsigned int num_workers):
  running_(true),
  active_worker_count_(0)
{
  for (unsigned int i = 0; i < num_workers; ++i)
  {
    workers_.emplace_back([&](){
        while (running_)
        {
          ++active_worker_count_;

          auto task = getNextTask();
          while (task)
          {
            auto work = task.value();
            work();

            task = getNextTask();
          }

          --active_worker_count_;
          if (active_worker_count_ == 0)
          {
            work_finished_.notify_all();
          }

          std::unique_lock<std::mutex> lk(work_lock_);
          work_available_.wait(lk);
          lk.unlock();
        }
      });
  }
}

ThreadPool::~ThreadPool()
{
  running_ = false;

  clearQueue();

  work_available_.notify_all();
  work_finished_.notify_all();

  for (auto& worker : workers_)
  {
    worker.join();
  }
}

void ThreadPool::clearQueue()
{
  std::unique_lock<std::mutex> lock(work_lock_);
  std::queue<ThreadPool::Task> empty;
  tasks_.swap(empty);
}

void ThreadPool::addWork(const std::function<void()>& task)
{
  std::unique_lock<std::mutex> lock(work_lock_);
  tasks_.emplace(task);
  work_available_.notify_all();
}

void ThreadPool::waitUntilFinished() const
{
  while (running_)
  {
    std::unique_lock<std::mutex> lk(work_lock_);
    work_finished_.wait(lk);

    if (tasks_.empty() && (active_worker_count_ == 0))
    {
      return;
    }

    lk.unlock();
  }
}

std::optional<ThreadPool::Task> ThreadPool::getNextTask()
{
  std::unique_lock<std::mutex> lock(work_lock_);
  if (tasks_.empty())
  {
    return std::nullopt;
  }

  auto task = std::optional<ThreadPool::Task>(tasks_.front());
  tasks_.pop();
  return task;
}