#include "pch.h"
#include "ComponentSystem.h"
#include "ComponentBase.h"

#include "ResourceLoader.h"
#include "MeshFilter.h"
#include "Animation.h"
#include "SkinningAnimation.h"

#include "ResourceObserverOwner.h"
#include "IResourceManager.h"

ComponentSystem::ComponentSystem()
{
	/// <summary>
	/// ������� �ʿ��� Ÿ�Լ��� 10�� �̳��� ���̴�.
	/// </summary>
	m_Component_V.resize(10);
	m_pResourceObserverOwner = std::make_shared<ResourceObserverOwner>();
}

ComponentSystem::~ComponentSystem()
{

}

void ComponentSystem::RegisterComponent(int type, ComponentBase* component)
{

	// Observer�� ��ӹ��� Ŭ�������� ���ο��� Ȯ���Ѵ�
	m_pResourceObserverOwner->RegisterObserver(component);
	

	///2d vector ver
	///type��° vector�� �����Ѵ�.
	/// render component�� ���� ����
	if (type == static_cast<int>(ComponentType::Rendering))
	{
		m_RenderComponent_V.push_back(component);
	}
	/// �׽�Ʈ��
	else if (type == static_cast<int>(ComponentType::Physics))
	{
		m_RenderComponent_V.push_back(component);
	}
	else if (type == static_cast<int>(ComponentType::Light))
	{
		m_LightComponent_V.push_back(component);
	}
	else
	{
		m_Component_V.at(type).push_back(std::make_pair(type, component));
	} 

	CA_TRACE("Component System : Register %d", component->GetComponetId());
	/// <summary>
	/// multimap ver
	/// </summary> 
	/// <param name="type"></param>
	/// <param name="component"></param>
	//m_Component_MM.insert(std::make_pair(type, component));

	//////////////////////////////////////////////////////////////////////////
}

void ComponentSystem::DeregisterComponent(int type, ComponentBase* component)
{
	/// <summary>
	/// ����������Ʈ ���Ϳ� �������� �������� ������ �ݴϴ�. �ӽù���
	/// </summary>
	if (type == static_cast<int>(ComponentType::Rendering)|| static_cast<int>(ComponentType::Physics))
	{
		std::vector<ComponentBase*>::iterator _rCompV = m_RenderComponent_V.begin();
		for (; _rCompV != m_RenderComponent_V.end(); _rCompV++)
		{
			if (*_rCompV == component)
			{
				m_RenderComponent_V.erase(_rCompV);
				CA_TRACE("Component System : Deregister %d", component->GetComponetId());
				return;
			}
		}
	}

	if (type == static_cast<int>(ComponentType::Light))
	{
		std::vector<ComponentBase*>::iterator _rCompV = m_LightComponent_V.begin();
		for (; _rCompV != m_LightComponent_V.end(); _rCompV++)
		{
			if (*_rCompV == component)
			{
				m_LightComponent_V.erase(_rCompV);
				CA_TRACE("Component System : Deregister %d", component->GetComponetId());
				return;
			}
		}
	}


	std::vector<std::pair<int, ComponentBase*>>::iterator _pairVIter = m_Component_V.at(type).begin();

	for (; _pairVIter != m_Component_V.at(type).end(); _pairVIter++)
	{
		if (_pairVIter->second == component)
		{
			m_Component_V.at(type).erase(_pairVIter);
			CA_TRACE("Component System : Deregister %d", component->GetComponetId());
			return;
		}
	}

	//std::multimap<int, ComponentBase*>::iterator _componentIter;

	//for (_componentIter = m_Component_MM.equal_range(type).first;
	//	_componentIter != m_Component_MM.equal_range(type).second;
	//	_componentIter++)
	//{
	//	if (_componentIter->second == component)
	//	{
	//		m_Component_MM.erase(_componentIter);
	//		return;
	//	}
	//}

	CA_TRACE("Component System : %d Deregister Fail", component->GetComponetId());
}

void ComponentSystem::DeregisterComponent(ComponentBase* component)
{
	DeregisterComponent(component->GetComponentType(), component);

	//for (auto _componentV : m_Component_V)
	//{
	//	std::vector<std::pair<int, ComponentBase*>>::iterator _pairVIter = _componentV.begin();

	//	for (; _pairVIter != _componentV.end(); _pairVIter++)
	//	{
	//		if (_pairVIter->second == component)
	//		{
	//			_componentV.erase(_pairVIter);
	//			CA_TRACE("Component System : Deregister %d", component->GetComponetId());

	//			return;
	//		}
	//	}
	//}

	//std::vector<ComponentBase*>::iterator _rCompV = m_RenderComponent_V.begin();
	//for (; _rCompV != m_RenderComponent_V.end(); _rCompV++)
	//{
	//	if (*_rCompV == component)
	//	{
	//		m_RenderComponent_V.erase(_rCompV);
	//		CA_TRACE("Component System : Deregister %d", component->GetComponetId());

	//		return;
	//	}
	//}

}

void ComponentSystem::DeregisterComponent(GameObject* object)
{

	for (ComponentBase* _comp : object->m_Component_V)
	{
		DeregisterComponent(_comp);
	}
}

void ComponentSystem::SetResourceLoader(ResourceLoader* pResourceLoader)
{
	m_pResourceLoader = pResourceLoader;
}

void ComponentSystem::SetResourceManager(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_pResourceManager = pResourceManager;
	m_pResourceObserverOwner->SetResourceManager(pResourceManager);
}

void ComponentSystem::Start()
{
	m_pResourceObserverOwner->Notify();


	std::vector<ComponentBase*>::iterator _rCompV = m_RenderComponent_V.begin();
	for (; _rCompV != m_RenderComponent_V.end(); _rCompV++)
	{
		(*_rCompV)->Start();
	}

	_rCompV = m_LightComponent_V.begin();
	for (; _rCompV != m_LightComponent_V.end(); _rCompV++)
	{
		(*_rCompV)->Start();
	}


	///
	/// auto���� ���۷��� ����� �����غ���, ����
	///
	for (auto _componentV : m_Component_V)
	{
		std::vector<std::pair<int, ComponentBase*>>::iterator _pairVIter = _componentV.begin();

		for (; _pairVIter != _componentV.end(); _pairVIter++)
		{
			if (_pairVIter->second->GetObjectIsExist())
			{
				_pairVIter->second->Start();
			}
			else
			{
				//��ŸƮ �������� �ڽ��� ������Ʈ�� ���� ������Ʈ�� �ý��ۿ��� ���ܵȴ�.
				CA_TRACE("Component System : %d No Object", _pairVIter->second->GetComponetId());
				_componentV.erase(_pairVIter);
			}
		}
	}

}

void ComponentSystem::Update(float dTime)
{
	//������Ʈ Ÿ�Ժ� ������Ʈ ������ �����غ���.
	for (int _type = 1; _type < m_Component_V.size(); _type++)
	{
		UpdateByType(dTime, _type);
	}
}


void ComponentSystem::LightPreRender()
{

	for (ComponentBase* _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnPreRender();
		}
	}
	

}

void ComponentSystem::PreRender()
{
	// ����Ʈ�� PreRender�� ������
	for (ComponentBase* _rComp : m_LightComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnPreRender();
		}
	}
}

void ComponentSystem::Render()
{
	for (ComponentBase* _rComp : m_RenderComponent_V)
	{
		if (_rComp->GetObjectIsEnabled() == true)
		{
			_rComp->OnRender();
		}
	}
}

void ComponentSystem::UpdateByType(float dTime, int type)
{

	///2d vector ver

	for (auto _pairV : m_Component_V.at(type))
	{
		if (_pairV.second->GetObjectIsEnabled() == true)
		{
			if (_pairV.second->GetIsEnabled() == true)
			{
				_pairV.second->Update(dTime);
			}
		}
	}

	/// <summary>
	/// multimap ver
	/// </summary>
	/// <param name="type"></param>
	/*std::multimap<int, ComponentBase*>::iterator _componentIter;

	for (_componentIter = m_Component_MM.equal_range(type).first;
		_componentIter != m_Component_MM.equal_range(type).second;
		_componentIter++)
	{
		if (_componentIter->second->GetObjectIsEnabled() == true)
		{
			if (_componentIter->second->GetIsEnabled() == true)
			{
				_componentIter->second->Update(dTime);
			}
		}
	}*/

	//////////////////////////////////////////////////////////////////////////
}
