#pragma once
#include <vector>
#include "GameObject.h"	
#include "JsonSerializer.h"

#define IDCOMPONENT 3

/// <summary>
/// Component /// ����Ƽ���� MonoBehavior�� �ش��ϴ� Ŭ������ ������ʹ�.
/// </summary>

//�ٸ� ������ ĳ�����ؼ� ����� �̷���� ���� ������ �Ժη� �ٲ��� �����ּ���.
enum class ComponentType
{
	Rendering = -1,

	Physics = 1,
	Input = 2,
	GameLogic = 3,
	Etc = 4,

	MeshFilter = 5,
	Animation,
	Light,
};

class GameObject;
class ComponentSystem;

class Transform;

class ComponentBase : public JsonSerializer
{
private:
	ComponentBase();
public:
	ComponentBase(ComponentType type);
	virtual ~ComponentBase();

public:
	virtual void Start() abstract;					//Start�� Update �޼��尡 ó�� ȣ��Ǳ� ����, ��ũ��Ʈ�� Ȱ��ȭ �Ǹ� ȣ��Ǿ�� �Ѵ�. 
	virtual void Update(float dTime) abstract;		//fixedUpdate(����������)�� ��������� ���� ������?

	virtual void OnPreRender();			// ��� ������Ʈ�� ��� ���� �ʿ䰡 ��� abstract�� �Ⱥ���
	virtual void OnRender() abstract;
	virtual void OnPostRender();
	


	void Remove();

	void OnEnable();
	void OnDisable();
	void SetEnable(const bool& enalble);

	bool GetIsEnabled();
private:
	bool m_bEnable;

	//public:
	//	bool GetIsRegistered();
	//private:
	//	bool m_bIsRegistered;

protected:
	const ComponentType m_Type;
	const unsigned int m_ComponentId;

private:
	unsigned int SetComponentId();				//����� ���Ǽ��� ���� id �ý���
public:
	const unsigned int GetComponetId();
	const int GetComponentType();

	//static unsigned int s_ComponentCnt;			
public:
	void SetMyObject(GameObject* object);
	bool GetObjectIsEnabled();
	bool GetObjectIsExist();
	GameObject* GetMyObject();
protected:
	GameObject* m_pMyObject; // �ڽ��� ����ִ� ������Ʈ�� �˾ƾ��� ��) physics���� transform �����Ҷ�

public:
	template <typename T>
	T* GetComponent();
	Transform* m_Transform;
	Transform* GetTransform()
	{
		if (m_pMyObject != nullptr)
		{
			return m_pMyObject->GetTransform();
		}
		else
		{
			//CA_TRACE("Component : No Object Component", m_ComponentId);
			return nullptr;
		}
	}

	///Json
	virtual void Save() final;
	virtual void Load();

	//virtual void SaveData() abstract;		///������ ������ �غ� �Ǹ� �߻����� �ٲܰ��Դϴ�.
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

};

template <typename T>
T* ComponentBase::GetComponent()
{
	if (m_pMyObject != nullptr)
	{
		return m_pMyObject->template GetComponent<T>();
	}

	return nullptr;
}

//unsigned int ComponentBase::s_ComponentCnt = 0;