
#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" {

////////////////////////////////////////////////////////////////
// C# struct types
////////////////////////////////////////////////////////////////

struct Vector3 {
  float x;
  float y;
  float z;
};

////////////////////////////////////////////////////////////////
// C# functions for C++ to call
////////////////////////////////////////////////////////////////

void (*MGCP_Tester_UnityDebugLog)(const char*);
int (*MGCP_Tester_GameObjectNew)();
int (*MGCP_Tester_GameObjectGetTransform)(int thisHandle);
void (*MGCP_Tester_TransformSetPosition)(int thisHandle, Vector3 position);

////////////////////////////////////////////////////////////////
// C++ functions for C# to call
////////////////////////////////////////////////////////////////

int numCreated;

DLLEXPORT void TesterInit(                      //
    void (*unityDebugLog)(const char*),         //
    int (*gameObjectNew)(),                     //
    int (*gameObjectGetTransform)(int),         //
    void (*transformSetPosition)(int, Vector3)  //
) {
  MGCP_Tester_UnityDebugLog = unityDebugLog;
  MGCP_Tester_GameObjectNew = gameObjectNew;
  MGCP_Tester_GameObjectGetTransform = gameObjectGetTransform;
  MGCP_Tester_TransformSetPosition = transformSetPosition;

  numCreated = 0;
}

DLLEXPORT void TesterMonoBehaviourUpdate(int thisHandle) {
  if (numCreated < 10) {
    int goHandle = MGCP_Tester_GameObjectNew();
    int transformHandle = MGCP_Tester_GameObjectGetTransform(goHandle);
    float comp = 10.0f * (float)numCreated;
    Vector3 position = {comp, comp, comp};
    MGCP_Tester_TransformSetPosition(transformHandle, position);
    numCreated++;
  }
}

}  // extern C