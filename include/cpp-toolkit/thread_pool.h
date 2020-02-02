#ifndef TheadPool_h
#define TheadPool_h

#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <optional>
#include <condition_variable>
#include <atomic>


class ThreadPool
{
public:
  ThreadPool(unsigned int num_workers);
  ~ThreadPool();

  void clearQueue();
  void addWork(const std::function<void()>& task);
  void waitUntilFinished() const;

private:
  using Task = std::function<void()>;
  std::optional<Task> getNextTask();

private:
  mutable std::mutex work_lock_;

  mutable std::condition_variable work_finished_;
  std::condition_variable work_available_;

  std::queue<Task> tasks_;

  std::atomic_bool running_;
  std::atomic<unsigned int> active_worker_count_;
  std::vector<std::thread> workers_;
};

#endif