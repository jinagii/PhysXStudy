#include "pch.h"
#include <winuser.h>

//#include "DLLDefine.h"
#include "SceneBase.h"

#include "Timer.h"
#include "Camera.h"
#include "Input.h"

#include "SceneManager.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "Managers.h"

#include "ComponentSystem.h"

#include "ResourceLoader.h"

#include "IRenderer.h"
#include "IResourceManager.h"
#include "InterfaceManager.h"

#include "Transform.h"
#include "MeshFilter.h"
#include "MaterialComponent.h"
#include "MeshRenderer.h"

#include "ImguiClass.h"
#include "imgui_impl_win32.h"

#include "AdapterManager.h"

#include "PhysicsEngine.h"

#include "CATrace.h"

#include "Engine.h"





Engine::Engine()
	: m_pRenderer(nullptr)
	, m_pResourceLoader(nullptr)
	, m_hWnd(nullptr)
	, m_pComponent(nullptr)
	, m_pInput(nullptr)
	, m_PrevMousePos({ 0, 0 })
	, m_CurrMousePos({ 0, 0 })
	, m_ClientWidth(0)
	, m_ClientHeight(0)
	, m_pInterfaces(nullptr)
	, m_TestImgui(nullptr)
	// test용은 초기화 안했음
{

}

Engine::~Engine()
{

}

void Engine::Initialize()
{
	// 렌더러 받아서 initialize한다

}

void Engine::Initialize(int hWND, int width, int height)
{
	m_hWnd = (HWND)hWND;

	m_ClientWidth = width;
	m_ClientHeight = height;

	Timer::GetInstance()->Reset();
	Timer::GetInstance()->Start();

	//m_pPhysicsEngine = new PhysicsEngine();
	//m_pPhysicsEngine->Initialize();


	// 매니저들 모아둔곳 이니셜라이즈
	Managers::GetInstance()->Initialize();
	Managers::GetInstance()->GetCameraManager()->SetHWND(m_hWnd);

	m_pInput = new Input();
	m_pInput->Initialize((HWND)hWND);

	m_pInterfaces = new InterfaceManager();


	m_pRenderer = m_pInterfaces->CreateRenderer();
	m_pRenderer->Initialize(hWND, width, height);


	m_pResourceManager = m_pInterfaces->CreateResourceManager();
	m_pResourceManager->Initialize(m_pRenderer);
	m_pRenderer->SetResourceManager(m_pResourceManager);

	m_TestImgui = new ImguiClass();
	m_TestImgui->Initialize(m_hWnd, m_pRenderer.get());


	m_pResourceLoader = new ResourceLoader();
	m_pResourceLoader->Initialize(m_pResourceManager);
	m_pResourceLoader->LoadResource(std::string("../Data/"));


	m_pComponent = ComponentSystem::GetInstance();
	m_pComponent->SetResourceLoader(m_pResourceLoader);
	m_pComponent->SetResourceManager(m_pResourceManager);
}

void Engine::Loop()
{
	Timer::GetInstance()->Tick();

	if (Managers::GetInstance()->GetSceneManager()->GetNowScene()->IsPhysicsScene() == true)
	{
		Managers::GetInstance()->GetPhysicsEngine()->PhysicsLoop();
		//m_pPhysicsEngine->PhysicsLoop();
	}

	float dTime = Timer::GetInstance()->DeltaTime();

	if (Timer::GetInstance()->FixFrame(60.0f))
	{
		UpdateAll(dTime);

		RenderAll();

		Timer::GetInstance()->ResetDeltaTime();
	}

}

void Engine::UpdateAll(float dTime)
{

	Managers::GetInstance()->GetSceneManager()->UpdateNowScene(dTime);
	//Managers::GetInstance()->GetCameraManager()->UpdateNowCamera(dTime);

	Managers::GetInstance()->GetCameraManager()->SetLensOnResize(m_ClientWidth, m_ClientHeight);

	m_pComponent->Update(dTime);

	InputUpdate(dTime);

	//_testCamera->UpdateViewMatrix();
}

void Engine::RenderAll()
{

	m_pComponent->PreRender();

	m_pRenderer->BeginDraw();

	DirectX::XMMATRIX _world =
		DirectX::XMMatrixScaling(1, 1, 1) *
		DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0) *
		DirectX::XMMatrixTranslation(0, 0, 0);

	m_pComponent->Render();

	m_TestImgui->Render();

	m_pRenderer->EndDraw();

}

void Engine::AddScene(std::string name, SceneBase* pScene)
{
	Managers::GetInstance()->GetSceneManager()->AddScene(name, pScene);

	// 물리씬이면 넣어줌
	if (pScene->IsPhysicsScene() == true)
	{
		Managers::GetInstance()->GetPhysicsEngine()->AddPhysicsScene(pScene);
	}
}

void Engine::SelectScene(std::string name)
{
	Managers::GetInstance()->GetSceneManager()->SetNowScene(name);
	//m_pPhysicsEngine->ReleasePhysicsActor();
	//if (Managers::GetInstance()->GetSceneManager()->GetNowScene()->IsPhysicsScene() == true)
	//{
	//   Managers::GetInstance()->GetSceneManager()->SetNowScene(name);
	//   //m_pPhysicsEngine->SetNowPhysicsScene(name);
	//}
}

void Engine::Finalize()
{

}

//void Engine::PushComponentToSystem()
//{
//   for (ObjectBase* _object : Managers::GetInstance()->GetObjectManager()->m_Object_V)
//   {
//      for (ComponentBase* _comp : _object->m_Component_V)
//      {
//         m_pComponent->RegisterComponent(_comp->GetComponentType(), _comp);
//      }
//   }
//
//   m_pComponent->Start();
//}

Input* Engine::GetInput()
{
	return m_pInput;
}

void Engine::InputUpdate(float dTime)
{
	m_pInput->KeyUpdate();
	m_pInput->MouseUpdate();
}

void Engine::OnResize(int width, int height)
{
	m_ClientWidth = width;
	m_ClientHeight = height;

	if (Managers::GetInstance()->GetCameraManager() != nullptr)
	{
		Managers::GetInstance()->GetCameraManager()->SetLensOnResize(width, height);

	}


	if (m_pRenderer != nullptr)
	{
		m_pRenderer->OnResize(width, height);
	}

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

std::shared_ptr<IRenderer> Engine::GetIRenderer() const
{
	return m_pRenderer;
}

ResourceLoader* Engine::GetResoruceLoader()
{
	return m_pResourceLoader;
}

