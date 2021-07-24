// This is an external file intended to be ported into a C# interop

#include "timemanager.h"

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" {

void (*UnityDebugLog)(const char*);

std::shared_ptr<mgcp::TimeManager> timeManager;
int count = 0;
int timeindex = 0;

DLLEXPORT void InitTimeManager(         //
    void (*unityDebugLog)(const char*)  //
) {
  UnityDebugLog = unityDebugLog;
  UnityDebugLog("cpp::InitTimeManager::");
  timeManager = std::make_shared<mgcp::TimeManager>(UnityDebugLog);
  timeManager->Start();
  // timeindex = timeManager->SetInterval(
  //     [&]() {
  //       count++;
  //       std::string log("cpp::SetInterval fired count: ");
  //       log.append(std::to_string(count));
  //       UnityDebugLog(log.c_str());
  //       if (count > 2) {
  //         timeManager->ClearTimeout(timeindex);
  //         return;
  //       }
  //     },
  //     1000);
}

DLLEXPORT void TimeManagerUpdate() { timeManager->Update(); }

DLLEXPORT void TimeManagerDestruct() {
  UnityDebugLog("cpp::TimeManagerDestruct::");
  timeManager->ClearAll();
  timeManager->Stop();
}

DLLEXPORT int32_t SetTimeout(void (*callback)(), int32_t time) {
  std::string log("cpp::SetTimeout made with :: interval: ");
  log.append(std::to_string(time));
  UnityDebugLog(log.c_str());
  return timeManager->SetTimeout(callback, time);
}

DLLEXPORT int32_t SetInterval(void (*callback)(), int32_t time) {
  std::string log("cpp::SetInterval made with :: interval: ");
  log.append(std::to_string(time));
  UnityDebugLog(log.c_str());
  return timeManager->SetInterval(callback, time);
}

DLLEXPORT void ClearTimer(int32_t keyIndex) {
  std::string log("cpp::ClearTimer made with :: keyIndex: ");
  log.append(std::to_string(keyIndex));
  UnityDebugLog(log.c_str());
  timeManager->ClearTimeout(keyIndex);
}

DLLEXPORT void ClearAll() {
  UnityDebugLog("cpp::ClearAll::");
  timeManager->ClearAll();
}

}  // extern C