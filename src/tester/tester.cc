
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

void (*UnityDebugLog)(const char*);
int (*GameObjectNew)();
int (*GameObjectGetTransform)(int thisHandle);
void (*TransformSetPosition)(int thisHandle, Vector3 position);

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
  UnityDebugLog = unityDebugLog;
  GameObjectNew = gameObjectNew;
  GameObjectGetTransform = gameObjectGetTransform;
  TransformSetPosition = transformSetPosition;

  numCreated = 0;
}

DLLEXPORT void TesterMonoBehaviourUpdate(int thisHandle) {
  if (numCreated < 10) {
    int goHandle = GameObjectNew();
    int transformHandle = GameObjectGetTransform(goHandle);
    float comp = 10.0f * (float)numCreated;
    Vector3 position = {comp, comp, comp};
    TransformSetPosition(transformHandle, position);
    numCreated++;
  }
}

}  // extern C