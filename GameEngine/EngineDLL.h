#pragma once
#include "DLLDefine.h"

#include "GameObject.h"
#include "ObjectManager.h"
#include "Transform.h"
#include "SceneBase.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "CameraManager.h"
#include "MeshFilter.h"
#include "Input.h"
#include "AnimationSystem.h"
#include "Raycast.h"
#include "PhysicsActor.h"

#include "Singleton.h"
#include "Timer.h"
#include "Managers.h"
#include "Engine.h"
#include <string>

/// Client로 제공되는 DLL함수들 집합소

namespace DLLTime
{
   extern "C"
   {
      _DLL float DeltaTime();
      _DLL float TotalTime();
      _DLL void Reset();
      _DLL void Start();
      _DLL void Stop();
      _DLL void Tick();
   }
}

namespace DLLEngine
{
   extern "C"
   {
      _DLL void CreateEngine();
      _DLL void Initialize(int hWND, int width, int hight);
      _DLL void Loop();
      _DLL void StartComponent(SceneBase* pScene);
      _DLL void RemoveComponent(SceneBase* pScene);
      _DLL void AddScene(std::string name, SceneBase* pScene);
      _DLL void SelectScene(std::string name);
      _DLL void OnResize(int width, int height);
      _DLL void WriteScene(SceneBase* pScene);

      _DLL void AddGameObjectToScene(Scene* scene, GameObject* gameobject);
      _DLL void RegisterObjectToObjectManager(std::vector<GameObject*>& gameobject);
      _DLL Scene* CreateScene(std::string sceneName);
      _DLL void SetParsingData(Scene* scene, std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);
      _DLL void SetValue(JsonSerializer* json, Json::Value* value);

      _DLL GameObject* CreateObject(bool isContinual = false);
      _DLL GameObject* CreateObject_NoTransform(bool isContinual = false);
      _DLL GameObject* FindGameObjectByName(const std::string& objectName);
      _DLL GameObject* FindGameObjectById(const unsigned int& objectId);
      _DLL Scene* CreateScene(std::string sceneName);
      _DLL void ResetAllObject();
      _DLL void SetNowCamera(std::string name);
      _DLL void RegisterAllObjectComponents();
      _DLL void RegisterObjectComponents();
      _DLL void SetupParentChild();

      _DLL void AddPhysicsActor(PhysicsActor* actor);
      _DLL void SetPhysicsTransform(Transform* transform);
      _DLL void SetTransform(GameObject* object, const Transform& pos);
      _DLL void SetVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel);
      _DLL void SetAngularVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel);
      //_DLL GameObject* RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
      //                                 DirectX::SimpleMath::Vector3 rayDir,
      //                                 GeometryBase* geometry, Transform* transform,
      //                                 float maxDist, int hitFlag);
      _DLL GameObject* RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin,
                                              const DirectX::SimpleMath::Vector3& dir, float dist);
      _DLL RaycastHit CheckRaycastHit(const DirectX::SimpleMath::Vector3& origin,
                                      const DirectX::SimpleMath::Vector3& dir, float dist);
      _DLL void CheckCollision(std::vector<CollisionHit>* data, PhysicsActor* actor, Transform* transform);

      _DLL Engine* GetEngine();
      _DLL HRESULT ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

   }
}

namespace DLLInput
{
   extern "C"
   {
      _DLL bool InputKey(int vk);
      _DLL bool InputKeyDown(int vk);
      _DLL bool InputKeyUp(int vk);
      _DLL POINT GetMouseClientPos();
      _DLL POINT GetPrevMouseClientPos();

   }
}