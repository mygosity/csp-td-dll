#include "threadpool.h"

namespace CodeBlacksmith {

ThreadPool::ThreadPool(std::size_t numThreads) : DynamicObject("ThreadPool") { Start(numThreads); }

ThreadPool::~ThreadPool() { Stop(); }

void ThreadPool::InvokeMethod(std::string& methodKey) {}

void ThreadPool::Start(std::size_t numThreads) {
  for (auto i = 0u; i < numThreads; ++i) {
    m_Threads.emplace_back([=] {
      while (true) {
        Task task;
        {
          std::unique_lock<std::mutex> lock{m_EventMutex};
          m_EventVar.wait(lock, [=] { return m_Stopping || !m_Tasks.empty(); });
          if (m_Stopping && m_Tasks.empty()) {
            break;
          }
          task = std::move(m_Tasks.front());
          m_Tasks.pop();
        }
        task();
      }
    });
  }
}

void ThreadPool::Stop() noexcept {
  {
    std::unique_lock<std::mutex> lock{m_EventMutex};
    m_Stopping = true;
  }
  m_EventVar.notify_all();
  for (auto& thread : m_Threads) {
    thread.join();
  }
}

}  // namespace CodeBlacksmith