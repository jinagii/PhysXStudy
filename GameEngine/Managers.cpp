#include "pch.h"
#include "Managers.h"



Managers::Managers()
   : m_pSceneManager(nullptr)
   , m_pObjectManager(nullptr)
   , m_pCameraManager(nullptr)
   , m_pComponentSystem(nullptr)
   , m_pPhysicsEngine(nullptr)
	, m_pJsonManager(nullptr)
{
}

Managers::~Managers()
{
}

SceneManager* Managers::GetSceneManager()
{
   return m_pSceneManager;
}

ObjectManager* Managers::GetObjectManager()
{
   return m_pObjectManager;
}

CameraManager* Managers::GetCameraManager()
{
   return m_pCameraManager;
}

ComponentSystem* Managers::GetComponentSystem()
{
	return m_pComponentSystem;
}

JsonManager* Managers::GetJsonManager()
{
	return m_pJsonManager;
}

PhysicsEngine* Managers::GetPhysicsEngine()
{
   return m_pPhysicsEngine;
}

void Managers::Initialize()
{
   m_pSceneManager = new SceneManager();
   m_pObjectManager = new ObjectManager();
   m_pCameraManager = new CameraManager();
   m_pJsonManager = new JsonManager();
   m_pPhysicsEngine = new PhysicsEngine();
   m_pPhysicsEngine->Initialize();
   m_pComponentSystem = ComponentSystem::GetInstance();
}

void Managers::GetManagersToClient(SceneManager* pSM, ObjectManager* pOM, CameraManager* pCM, ComponentSystem* pCS)
{
   m_pSceneManager = pSM;
   m_pObjectManager = pOM;
   m_pCameraManager = pCM;
   m_pComponentSystem = pCS;
}
