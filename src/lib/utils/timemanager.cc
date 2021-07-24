#include "timemanager.h"

#include <iostream>
#include <thread>

namespace mgcp {

/*****************************************************************************************************************************************
 * TimerObject
 *****************************************************************************************************************************************/
TimerObject::TimerObject(int64_t nt, std::function<void()> cb, int32_t interval)
    : nextTime(nt), callback(std::move(cb)), interval(interval) {}
TimerObject::TimerObject(int64_t nt, std::function<void()> cb, int32_t interval, int32_t keyIndex, bool repeat)
    : nextTime(nt), callback(std::move(cb)), interval(interval), keyIndex(keyIndex), repeat(repeat) {}

TimerObject::~TimerObject() {
  stdlog("TimerObject Destroyed - setting callback to null");
  callback = nullptr;
}
/*****************************************************************************************************************************************
 * TimeManager
 *****************************************************************************************************************************************/
TimeManager::TimeManager(std::function<void(const char*)> loggingCallback) : extLog(loggingCallback) {
  stdlog("TimeManager created");
  if (extLog)
    extLog("TimeManager created");
}

TimeManager::~TimeManager() {
  stdlog("TimeManager Destroyed");
  if (extLog)
    extLog("TimeManager Destoyed");
  Stop();
  extLog = nullptr;
  m_timeoutList.clear();
}

void TimeManager::Start() {
  if (m_activeThread)
    return;
  m_activeThread = true;
  std::thread([&, this]() {
    while (true) {
      // std::unique_lock<std::mutex> lock{m_mutex};
      // //OSX - 57% is 1micro, 25% - 10micro - 5% 100micro
      auto time = 100;
      std::this_thread::sleep_for(std::chrono::microseconds(time));
      if (!m_activeThread) {
        return;
      }
      Update();
    }
  }).detach();
}

void TimeManager::Stop() {
  // std::unique_lock<std::mutex> lock{m_mutex};
  m_activeThread = false;
  extLog = nullptr;
}

void TimeManager::Update() {
  std::unique_lock<std::mutex> lock{m_mutex};
  int64_t timestamp = GetTimeNow();
  if (timestamp >= m_nextUpdateTime && m_timeoutList.size() > 0) {
    int64_t nextShortestTime = std::numeric_limits<int64_t>::max();
    for (int32_t i = (int32_t)m_timeoutList.size() - 1; i >= 0; --i) {
      auto& c = m_timeoutList.at(i);
      if (c.deferErasure) {
        m_timeoutList.erase(m_timeoutList.begin() + i);
      } else if (timestamp >= c.nextTime) {
        c.callback();
        if (c.repeat) {
          c.nextTime = timestamp + c.interval;
        } else {
          c.nextTime = std::numeric_limits<int64_t>::max();
        }
      }
      nextShortestTime = nextShortestTime <= c.nextTime ? nextShortestTime : c.nextTime;
    }
    m_nextUpdateTime = nextShortestTime <= m_nextUpdateTime ? nextShortestTime : m_nextUpdateTime;
  }
}

int64_t TimeManager::GetNextIntervalTime() { return m_nextUpdateTime - GetTimeNow(); }

int32_t TimeManager::GetNextKey() { return m_keyIndex; }

int32_t TimeManager::SetTimeout(std::function<void()> callback, int32_t interval) {
  // stdlog("SetTimeout:: interval: " << interval);
  AddTimerObject(m_keyIndex, GetTimeNow() + interval, std::move(callback), interval, 0);
  return m_keyIndex++;
}

void TimeManager::SetOrUpdateTimeout(std::function<void()> callback, int32_t interval, int32_t keyIndex) {
  auto nextTime = GetTimeNow() + interval;
  if (!UpdateTimerObject(keyIndex, nextTime, callback, interval, 0)) {
    AddTimerObject(keyIndex, GetTimeNow() + interval, std::move(callback), interval, 0);
  }
}

int32_t TimeManager::SetInterval(std::function<void()> callback, int32_t interval) {
  // stdlog("SetInterval:: interval: " << interval);
  AddTimerObject(m_keyIndex, GetTimeNow() + interval, std::move(callback), interval, 1);
  return m_keyIndex++;
}

void TimeManager::SetOrUpdateInterval(std::function<void()> callback, int32_t interval, int32_t keyIndex) {
  auto nextTime = GetTimeNow() + interval;
  if (!UpdateTimerObject(keyIndex, nextTime, callback, interval, 1)) {
    AddTimerObject(keyIndex, GetTimeNow() + interval, std::move(callback), interval, 1);
  }
}

void TimeManager::ClearTimeout(int32_t keyIndex) {
  stdlog("ClearTimeout::");
  if (extLog)
    extLog("ClearTimeout::");
  RemoveTimerObject(keyIndex);
}

void TimeManager::ClearInterval(int32_t keyIndex) {
  stdlog("ClearInterval::");
  if (extLog)
    extLog("ClearInterval::");
  RemoveTimerObject(keyIndex);
}

void TimeManager::ClearAll() {
  stdlog("ClearAll::");
  if (extLog)
    extLog("ClearAll::");
  std::unique_lock<std::mutex> lock{m_mutex};
  m_timeoutList.clear();
}

/*****************************************************************************************************************
Private functions
*****************************************************************************************************************/
void TimeManager::AddTimerObject(int32_t keyIndex, int64_t nextTime, std::function<void()> callback, int32_t interval, bool repeat) {
  std::unique_lock<std::mutex> lock{m_mutex};
  m_nextUpdateTime = nextTime <= m_nextUpdateTime ? nextTime : m_nextUpdateTime;
  stdlog("AddTimerObject:: callback adding new callback, interval: " << interval);
  m_timeoutList.emplace_back(nextTime, callback, interval, keyIndex, repeat);
  stdlog("time now: " << GetTimeNow() << " nextTime: " << nextTime << " m_nextUpdateTime: " << m_nextUpdateTime);
}

bool TimeManager::UpdateTimerObject(int32_t keyIndex, int64_t nextTime, std::function<void()> callback, int32_t interval, bool repeat) {
  std::unique_lock<std::mutex> lock{m_mutex};
  m_nextUpdateTime = nextTime <= m_nextUpdateTime ? nextTime : m_nextUpdateTime;
  for (TimerObject& c : m_timeoutList) {
    if (c.keyIndex == keyIndex) {
      stdlog("UpdateTimerObject:: callback exists modifying the next update time, interval: " << interval);
      c.interval = interval;
      c.nextTime = nextTime;
      c.repeat = repeat;
      c.callback = callback;
      return true;
    }
  }
  return false;
}

void TimeManager::PauseTimerObject(int32_t keyIndex) {
  std::unique_lock<std::mutex> lock{m_mutex};
  stdlog("PauseTimerObject::");
  for (int32_t i = 0; i < (int32_t)m_timeoutList.size(); ++i) {
    auto& c = m_timeoutList[i];
    if (c.keyIndex == keyIndex) {
      stdlog("PauseTimerObject:: resetting nextTime: " << std::numeric_limits<int64_t>::max());
      c.nextTime = std::numeric_limits<int64_t>::max();
      return;
    }
  }
}

void TimeManager::RemoveTimerObject(int32_t keyIndex) {
  std::unique_lock<std::mutex> lock{m_mutex};
  int32_t i = (int32_t)m_timeoutList.size() - 1;
  for (; i >= 0; --i) {
    auto& c = m_timeoutList[i];
    if (c.keyIndex == keyIndex) {
      if (m_mutex.try_lock()) {
        stdlog("RemoveTimerObject:: found callback and erasing it");
        if (extLog)
          extLog("RemoveTimerObject:: found callback and erasing it");
        m_timeoutList.erase(m_timeoutList.begin() + i);
        m_mutex.unlock();
      } else {
        stdlog("RemoveTimerObject:: locked out -> deferring erasure");
        if (extLog)
          extLog("RemoveTimerObject:: locked out -> deferring erasure");
        c.deferErasure = 1;
      }
      return;
    }
  }
}

}  // namespace mgcp
