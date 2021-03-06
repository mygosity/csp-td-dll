#ifndef MGCP_LIB_UTILS_TIMEMANAGER_H
#define MGCP_LIB_UTILS_TIMEMANAGER_H

#ifndef stdlog
#define stdlog(x) std::cout << x << std::endl;
#endif

#include <functional>
#include <mutex>
#include <vector>

#include "time.h"

namespace mgcp {

struct TimerObject {
  TimerObject(int64_t nt, std::function<void()> cb, int32_t interval);
  TimerObject(int64_t nt, std::function<void()> cb, int32_t interval, int32_t keyIndex, bool repeat);
  ~TimerObject();

  int64_t nextTime;                // 8
  std::function<void()> callback;  // 4
  int32_t interval;                // 4
  int32_t keyIndex;                // 4
  bool repeat = 0;                 // 1
  bool deferErasure = 0;           // 1
};

class TimeManager {
 public:
  TimeManager(std::function<void(const char*)> loggingCallback);
  ~TimeManager();

  void Start();
  void Stop();
  void Update();

  int32_t GetNextKey();

  int32_t SetTimeout(std::function<void()> callback, int32_t interval);
  void SetOrUpdateTimeout(std::function<void()> callback, int32_t interval, int32_t keyIndex);

  int32_t SetInterval(std::function<void()> callback, int32_t interval);
  void SetOrUpdateInterval(std::function<void()> callback, int32_t interval, int32_t keyIndex);

  void ClearTimeout(int32_t keyIndex);
  void ClearInterval(int32_t keyIndex);
  void ClearAll();

  int64_t GetNextIntervalTime();

 private:
  std::function<void(const char*)> extLog;
  int32_t m_keyIndex = 0;
  int64_t m_nextUpdateTime = 0;
  std::vector<TimerObject> m_timeoutList;

  void AddTimerObject(int32_t keyIndex, int64_t nextTime, std::function<void()> callback, int32_t interval, bool repeat);
  bool UpdateTimerObject(int32_t keyIndex, int64_t nextTime, std::function<void()> callback, int32_t interval, bool repeat);
  void PauseTimerObject(int32_t keyIndex);
  void RemoveTimerObject(int32_t keyIndex);

  std::mutex m_mutex;
  bool m_activeThread = false;
};

}  // namespace mgcp

#endif