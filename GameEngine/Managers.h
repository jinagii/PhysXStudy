#pragma once
#include "Singleton.h"

#include "CameraManager.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "ComponentSystem.h"
#include "PhysicsEngine.h"
#include "JsonManager.h"

class SceneManager;
class ObjectManager;
class CameraManager;
class ComponentSystem;
class PhysicsEngine;
class ResourceLoader;

/// 모든 매니저들을 관리하는 클래스

class Managers : public Singleton<Managers>
{
public:
   Managers();
   ~Managers();

public:
   void Initialize();

   void GetManagersToClient(SceneManager*, ObjectManager*, CameraManager*, ComponentSystem*);

   SceneManager* GetSceneManager();
   ObjectManager* GetObjectManager();
   CameraManager* GetCameraManager();
   ComponentSystem* GetComponentSystem();
   JsonManager* GetJsonManager();
   PhysicsEngine* GetPhysicsEngine();

private:
   SceneManager* m_pSceneManager;
   ObjectManager* m_pObjectManager;
   CameraManager* m_pCameraManager;
   ComponentSystem* m_pComponentSystem;
   JsonManager* m_pJsonManager;
   PhysicsEngine* m_pPhysicsEngine;
};

