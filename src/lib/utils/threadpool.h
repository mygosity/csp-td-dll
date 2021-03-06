#ifndef MGCP_LIB_UTILS_THREADPOOL_H
#define MGCP_LIB_UTILS_THREADPOOL_H

#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

#include "../types/dynamicobject.h"

namespace CodeBlacksmith {

class ThreadPool : DynamicObject {
 public:
  using Task = std::function<void()>;

  explicit ThreadPool(std::size_t numThreads);
  ~ThreadPool();

  virtual void InvokeMethod(std::string& methodKey) override;

  template <class T>
  auto Enqueue(T task) -> std::future<decltype(task())> {
    auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));
    {
      std::unique_lock<std::mutex> lock{m_EventMutex};
      m_Tasks.emplace([=] { (*wrapper)(); });
    }
    m_EventVar.notify_one();
    return wrapper->get_future();
  }

  // inline const std::string &getName() const override { return mName; };

 private:
  std::vector<std::thread> m_Threads;
  std::condition_variable m_EventVar;
  std::mutex m_EventMutex;
  bool m_Stopping = false;
  std::queue<Task> m_Tasks;

  void Start(std::size_t numThreads);
  void Stop() noexcept;
};

}  // namespace CodeBlacksmith

#endif