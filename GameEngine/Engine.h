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

/// ���״�� ����, Ŭ���� ��κ��� Update()�� ���⼭ UpdateAll() ����
/// ���� RenderAll()�� ����
/// DLL�� �����Ǵ� �Լ��鵵 ��κ� ���⼭ ����

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
	// ������ ��������
	//CComObject<IRenderer>* m_pRenderer;
	//PhysicsEngine* m_pPhysicsEngine;

	InterfaceManager* m_pInterfaces;

	

	std::shared_ptr<IRenderer> m_pRenderer;
	std::shared_ptr<IResourceManager> m_pResourceManager;
	ResourceLoader* m_pResourceLoader;

	HWND m_hWnd;

	// �Ŵ������� Managers���� �޾ƾ���
	ImguiClass* m_TestImgui;
	ComponentSystem* m_pComponent;

	Input* m_pInput;
	POINT m_CurrMousePos;
	POINT m_PrevMousePos;


	int m_ClientWidth;
	int m_ClientHeight;

	// test �뵵
   //GameObject* _testObject;
   //GameObject* _cameraObject;
   //Camera* _testCamera;

};

