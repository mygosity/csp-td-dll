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

DLLEXPORT void InitTimeManager(         //
    void (*unityDebugLog)(const char*)  //
) {
  UnityDebugLog = unityDebugLog;
  UnityDebugLog("InitTimeManager:: from c++");
  timeManager = std::make_shared<mgcp::TimeManager>();
}

DLLEXPORT void CleanUpTimeManager() {
  // delete timeManager;
}

}  // extern C