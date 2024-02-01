// This is an external file intended to be ported into a C# interop

#include "timemanager.h"

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" {

void (*MGCP_TM_UnityDebugLog)(const char*);

std::shared_ptr<mgcp::TimeManager> MGCP_TM_timeManager;
int MGCP_TM_count = 0;
int MGCP_TM_timeindex = 0;

DLLEXPORT void InitTimeManager(         //
    void (*unityDebugLog)(const char*)  //
) {
  MGCP_TM_UnityDebugLog = unityDebugLog;
  MGCP_TM_UnityDebugLog("cpp::InitTimeManager::");
  MGCP_TM_timeManager = std::make_shared<mgcp::TimeManager>(MGCP_TM_UnityDebugLog);
  MGCP_TM_timeManager->Start();
  // timeindex = timeManager->SetInterval(
  //     [&]() {
  //       count++;
  //       std::string log("cpp::SetInterval fired count: ");
  //       log.append(std::to_string(count));
  //       TM_UnityDebugLog(log.c_str());
  //       if (count > 2) {
  //         timeManager->ClearTimeout(timeindex);
  //         return;
  //       }
  //     },
  //     1000);
}

DLLEXPORT void TimeManagerUpdate() { MGCP_TM_timeManager->Update(); }

DLLEXPORT void TimeManagerDestruct() {
  MGCP_TM_UnityDebugLog("cpp::TimeManagerDestruct::");
  MGCP_TM_timeManager->ClearAll();
  MGCP_TM_timeManager->Stop();
}

DLLEXPORT int32_t SetTimeout(void (*callback)(), int32_t time) {
  std::string log("cpp::SetTimeout made with :: interval: ");
  log.append(std::to_string(time));
  MGCP_TM_UnityDebugLog(log.c_str());
  return MGCP_TM_timeManager->SetTimeout(callback, time);
}

DLLEXPORT int32_t SetInterval(void (*callback)(), int32_t time) {
  std::string log("cpp::SetInterval made with :: interval: ");
  log.append(std::to_string(time));
  MGCP_TM_UnityDebugLog(log.c_str());
  return MGCP_TM_timeManager->SetInterval(callback, time);
}

DLLEXPORT void ClearTimer(int32_t keyIndex) {
  std::string log("cpp::ClearTimer made with :: keyIndex: ");
  log.append(std::to_string(keyIndex));
  MGCP_TM_UnityDebugLog(log.c_str());
  MGCP_TM_timeManager->ClearTimeout(keyIndex);
}

DLLEXPORT void ClearAll() {
  MGCP_TM_UnityDebugLog("cpp::ClearAll::");
  MGCP_TM_timeManager->ClearAll();
}

}  // extern C