#pragma once
/// <summary>
/// ������Ʈ�� ������ �ô� Class 
/// </summary>
/// 
#include "Singletone.h"
#include <memory>
#include <map>
#include <vector>
#include <functional>

//#define PHYSICS 0
//#define ETC 1
#define RENDERING -1


class ComponentBase;
class GameObject;
class ResourceLoader;
class ResourceObserverOwner;
__interface IResourceManager;

class ComponentSystem: public Singletone<ComponentSystem>
{
public:
	ComponentSystem();
	~ComponentSystem();

public:
	void RegisterComponent(int type, ComponentBase* component);
	void DeregisterComponent(int type, ComponentBase* component);
	void DeregisterComponent(ComponentBase* component);				//Ÿ���� ���������� ������ ����� �� ��δ�.
	void DeregisterComponent(GameObject* object);

	void SetResourceLoader(ResourceLoader* pResourceLoader);
	void SetResourceManager(std::shared_ptr<IResourceManager> pResourceManager);


private:
	std::vector<std::vector<std::pair<int, ComponentBase*>>> m_Component_V;		//������Ʈ�� ���ϴ� ������� �����ؾ� �Ѵ�
	std::vector<ComponentBase*> m_RenderComponent_V;
	std::vector<ComponentBase*> m_LightComponent_V;

	std::shared_ptr<ResourceObserverOwner> m_pResourceObserverOwner;

	std::shared_ptr<IResourceManager> m_pResourceManager;
	ResourceLoader* m_pResourceLoader;
	//std::multimap<int, ComponentBase*> m_Component_MM;
public:
	void Start();
	void Update(float dTime);
	void LightPreRender();
	void PreRender();
	void Render();
private:
	void UpdateByType(float dTime, int type);

public:
	std::map<std::string, ComponentBase*> m_ComponentConstructor_M;
};

//ComponentSystem* g_pComponentSystem = ComponentSystem::GetInstance();