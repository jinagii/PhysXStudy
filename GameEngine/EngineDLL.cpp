#include "pch.h"
#include "EngineDLL.h"
#include "ResourceLoader.h"

static Engine* g_Engine = nullptr;

Timer* g_Timer = Timer::GetInstance();
Managers* g_Managers = Managers::GetInstance();


namespace DLLTime
{
	_DLL float DeltaTime()
	{
		return g_Timer->DeltaTime();
		//return g_Timer->_test;
	}

	_DLL float TotalTime()
	{
		return g_Timer->TotalTime();
	}

	_DLL void Reset()
	{
		g_Timer->Reset();
	}

	_DLL void Start()
	{
		g_Timer->Start();
	}

	_DLL void Stop()
	{
		g_Timer->Stop();
	}

	_DLL void Tick()
	{
		g_Timer->Tick();
	}
}

namespace DLLEngine
{
	_DLL void CreateEngine()
	{
		if (g_Engine == nullptr)
		{
			g_Engine = new Engine();
		}
	}

	_DLL void Initialize(int hWND, int width, int hight)
	{
		g_Engine->Initialize(hWND, width, hight);
	}

	_DLL void Loop()
	{
		g_Engine->Loop();
	}

	_DLL void AddScene(string name, SceneBase* pScene)
	{
		g_Engine->AddScene(name, pScene);
	}

	_DLL void SelectScene(string name)
	{
		g_Engine->SelectScene(name);
	}

	_DLL void OnResize(int width, int height)
	{
		g_Engine->OnResize(width, height);
	}

	_DLL GameObject* CreateObject(bool isContinual)
	{
		GameObject* _object = g_Managers->GetObjectManager()->CreateObject(isContinual);

		/// <summary>
		/// Transform컴포넌트를 기본으로 삽입하는 곳
		/// </summary>
		/// <returns></returns>
		Transform* _transformC = new Transform();
		_object->m_Transform = _transformC;
		_object->AddComponent<Transform>(_transformC);
		return _object;
	}

	_DLL GameObject* CreateObject_NoTransform(bool isContinual)
	{
		GameObject* _object = new GameObject(isContinual); //g_Managers->GetObjectManager()->CreateObject(isContinual);
		return _object;
	}

	_DLL void ResetAllObject()
	{
		g_Managers->GetObjectManager()->ResetAllObject();
	}

	_DLL void SetNowCamera(string name)
	{
		g_Managers->GetCameraManager()->SetNowCamera(name);
	}

	_DLL Engine* GetEngine()
	{
		return g_Engine;
	}
	_DLL Managers* GetManagers()
	{
		return g_Managers;
	}

	_DLL void StartComponent(SceneBase* pScene)
	{
		//pScene->StartComponent();
		g_Managers->GetSceneManager()->GetNowScene()->StartComponent();
	}

	_DLL void RemoveComponent(SceneBase* pScene)
	{
		pScene->ReleaseObject();
	}

	_DLL void RegisterAllObjectComponents()
	{
		g_Managers->GetObjectManager()->StartAllObject();
	}

	_DLL void RegisterObjectComponents()
	{
		g_Managers->GetObjectManager()->StartDisContinualObject();
	}

	_DLL void SetupParentChild()
	{
		g_Managers->GetObjectManager()->SetupParentChild();
	}

	_DLL void AddPhysicsActor(PhysicsActor* actor)
	{
		g_Managers->GetPhysicsEngine()->AddPhysicsActor(actor);
	}

	_DLL void SetPhysicsTransform(Transform* transform)
	{
		g_Managers->GetPhysicsEngine()->SetPhysicsTransform(transform);
	}

   _DLL void SetTransform(GameObject* object, const Transform& pos)
   {
      g_Managers->GetPhysicsEngine()->SetTransform(object, pos);
   }

   _DLL void SetVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel)
   {
		g_Managers->GetPhysicsEngine()->SetVelocity(object, vel);
   }

   _DLL void SetAngularVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel)
   {
      g_Managers->GetPhysicsEngine()->SetAngularVelocity(object, vel);
   }

   //_DLL GameObject* RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
			//											 DirectX::SimpleMath::Vector3 rayDir, 
			//											 GeometryBase* geometry, Transform* transform, 
			//											 float maxDist, int hitFlag)
   //{
   //   return g_Managers->GetPhysicsSystem()->RaycastCheckGameObject(rayOrigin, rayDir, geometry, transform, maxDist, hitFlag);
   //}

   _DLL GameObject* RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir, float dist)
   {
      return g_Managers->GetPhysicsEngine()->RaycastCheckGameObject(origin, dir, dist);
   }

   _DLL RaycastHit CheckRaycastHit(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir, float dist)
   {
		return g_Managers->GetPhysicsEngine()->CheckRaycastHit(origin, dir, dist);
   }

   _DLL void CheckCollision(std::vector<CollisionHit>* data, PhysicsActor* actor, Transform* transform)
   {
		g_Managers->GetPhysicsEngine()->CheckCollision(data, actor, transform);
   }


	_DLL HRESULT ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return g_Engine->ImGuiHandler(hWnd, msg, wParam, lParam);
	}

	_DLL void WriteScene(SceneBase* pScene)
	{
		pScene->WriteSceneData();
	}

	_DLL GameObject* FindGameObjectByName(const std::string& objectName)
	{
		return g_Managers->GetObjectManager()->FindGameObect(objectName);
	}

	_DLL GameObject* FindGameObjectById(const unsigned int& objectId)
	{
		return g_Managers->GetObjectManager()->FindGameObect(objectId);
	}

   _DLL Scene* CreateScene(std::string sceneName)
   {
      return new Scene(sceneName);
   }


   _DLL void AddGameObjectToScene(Scene* scene, GameObject* gameobject)
   {
      scene->AddObject(gameobject);
   }

   _DLL void SetParsingData(Scene* scene, std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
   {
      scene->SetParsingData(gameobject, component);
   }

   _DLL void SetValue(JsonSerializer* json, Json::Value* value)
   {
      json->SetValue(value);
   }

   _DLL void RegisterObjectToObjectManager(std::vector<GameObject*>& gameobject)
   {
      for (GameObject*& _go : gameobject)
      {
         Managers::GetInstance()->GetObjectManager()->AddToManager(_go);
      }
   }


}

namespace DLLInput
{
	_DLL bool InputKey(int vk)
	{
		return g_Engine->GetInput()->GetKey(vk);
	}

	_DLL bool InputKeyDown(int vk)
	{
		return g_Engine->GetInput()->GetKeyDown(vk);
	}

	_DLL bool InputKeyUp(int vk)
	{
		return g_Engine->GetInput()->GetKeyUp(vk);
	}

	_DLL POINT GetMouseClientPos()
	{
		return g_Engine->GetInput()->GetMouseClientPos();
	}

	_DLL POINT GetPrevMouseClientPos()
	{
		return g_Engine->GetInput()->GetPrevMouseClientPos();
	}

}