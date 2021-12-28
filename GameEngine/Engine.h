#pragma once

class SceneBase;

class SceneManager;
class ObjectManager;
class CameraManager;

class Timer;
class Camera;
class Input;

class GameObject;

__interface IRenderer;
__interface IResourceManager;

class InterfaceManager;
class ResourceLoader;
class ComponentSystem;

class MeshFilter;
class MaterialData;
class ImguiClass;

class PhysicsEngine;

/// 말그대로 엔진, 클래스 대부분의 Update()를 여기서 UpdateAll() 해줌
/// 이후 RenderAll()을 해줌
/// DLL로 제공되는 함수들도 대부분 여기서 관리

class Engine
{
public:
	Engine();
	~Engine();

	void Initialize();
	void Initialize(int hWND, int width, int height);

	void Loop();
	void UpdateAll(float dTime);
	void RenderAll();

	void AddScene(std::string name, SceneBase* pScene);
	void SelectScene(std::string name);

	void Finalize();

	//void PushComponentToSystem();

	Input* GetInput();
	void InputUpdate(float dTime);

	void OnResize(int width, int height);

	LRESULT ImGuiHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



   //void OnMouseDown(HWND hWnd, int x, int y);
   //void OnMouseUp();
	//void OnMouseMove(int btnState, int x, int y);

	std::shared_ptr<IRenderer> GetIRenderer() const;
	ResourceLoader* GetResoruceLoader();
	
private:
	// 렌더러 내놓으셈
	//CComObject<IRenderer>* m_pRenderer;
	//PhysicsEngine* m_pPhysicsEngine;

	InterfaceManager* m_pInterfaces;

	

	std::shared_ptr<IRenderer> m_pRenderer;
	std::shared_ptr<IResourceManager> m_pResourceManager;
	ResourceLoader* m_pResourceLoader;

	HWND m_hWnd;

	// 매니저들은 Managers에서 받아쓰자
	ImguiClass* m_TestImgui;
	ComponentSystem* m_pComponent;

	Input* m_pInput;
	POINT m_CurrMousePos;
	POINT m_PrevMousePos;


	int m_ClientWidth;
	int m_ClientHeight;

	// test 용도
   //GameObject* _testObject;
   //GameObject* _cameraObject;
   //Camera* _testCamera;

};

