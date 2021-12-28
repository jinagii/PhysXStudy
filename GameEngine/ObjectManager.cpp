#include "pch.h"
#include "ComponentSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Managers.h"
#include "ComponentSystem.h"
#include "ComponentBase.h"
#include "ObjectManager.h"

ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{

}

GameObject* ObjectManager::CreateObject(bool isContinual)
{
	GameObject* _object = new GameObject(isContinual);
	AddToManager(_object);
	return _object;
}

void ObjectManager::StartAllObject()
{
	for (GameObject* _object : m_Object_V)
	{
		for (ComponentBase* _co : _object->m_Component_V)
		{
			ComponentSystem::GetInstance()->RegisterComponent(_co->GetComponentType(), _co);
		}
		/// <summary>
		/// Camera compnent�� �ִ� ������Ʈ��� ī�޶󸮽�Ʈ�� ����
		/// </summary>
		AddCameraToManager(_object);
	}
}

void ObjectManager::StartDisContinualObject()
{
	for (GameObject* _object : m_Object_V)
	{
		if (_object->GetIsContinualObject() == true)
		{
			continue;
		}

		for (ComponentBase* _co : _object->m_Component_V)
		{
			ComponentSystem::GetInstance()->RegisterComponent(_co->GetComponentType(), _co);
		}
		/// <summary>
		/// Camera compnent�� �ִ� ������Ʈ��� ī�޶󸮽�Ʈ�� ����
		/// </summary>
		AddCameraToManager(_object);
	}
}

void ObjectManager::UpdateAllObject(float dTime)
{
	for (GameObject* _object : m_Object_V)
	{
		if (_object->GetIsEnabled() == true)
		{
			_object->Update(dTime);
		}
	}
}

void ObjectManager::ReleaseAllObject()
{
	std::vector<GameObject*>::iterator _objectIter = m_Object_V.begin();
	for (; _objectIter != m_Object_V.end();)
	{
		(*_objectIter)->Release();
		delete(*_objectIter);
		m_Object_V.erase(_objectIter);
	}
}

void ObjectManager::ReleaseObject()
{
	std::vector<GameObject*>::iterator _objectIter = m_Object_V.begin();
	for (; _objectIter != m_Object_V.end();)
	{
		if ((*_objectIter)->GetIsContinualObject() == false)
		{
			Managers::GetInstance()->GetComponentSystem()->DeregisterComponent(*_objectIter);
			(*_objectIter)->Release();
			delete(*_objectIter);
			_objectIter = m_Object_V.erase(_objectIter);
		}
		else
		{
			_objectIter++;
		}
	}
}

void ObjectManager::ResetAllObject()
{
	for (GameObject* _object : m_Object_V)
	{
		_object->Reset();
	}
}

void ObjectManager::SetupParentChild()
{
	for (GameObject* _object : m_Object_V)
	{
		// Transform�� �θ��ڽİ��� ������
		if (_object->GetParent() != nullptr)
		{
			_object->GetTransform()->SetParentTM(_object->GetParent());
		}
		/// Transform ���� �� ���� ���� ���� �߰�

	}
}

void ObjectManager::AddToManager(GameObject* object)
{
	m_Object_V.push_back(object);
}

void ObjectManager::AddCameraToManager(GameObject* cameraObject)
{
	Camera* _camera = cameraObject->GetComponent<Camera>();
	if (_camera!=nullptr)
	{
		m_Camera_V.push_back(_camera);
	}
}

GameObject* ObjectManager::FindGameObect(const std::string& objectName)
{
	for (GameObject* _go : m_Object_V)
	{
		if (_go->GetObjectName()==objectName)
		{
			return _go;
		}
	}

	CA_TRACE("Object Manager : ��ġ�ϴ� ������Ʈ�� �����ϴ�.");
	//���� �������� �������� �ʴ� ������Ʈ
	return nullptr;
}

GameObject* ObjectManager::FindGameObect(const unsigned int& objectId)
{
	for (GameObject* _go : m_Object_V)
	{
		if (_go->GetGameObjectId() == objectId)
		{
			return _go;
		}
	}

	CA_TRACE("Object Manager : ��ġ�ϴ� ������Ʈ�� �����ϴ�.");
	//���� �������� �������� �ʴ� ������Ʈ
	return nullptr;
}

void ObjectManager::SetNowCamera()
{

}

