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
	/// 순서제어가 필요한 타입수가 10개 이내일 것이다.
	/// </summary>
	m_Component_V.resize(10);
	m_pResourceObserverOwner = std::make_shared<ResourceObserverOwner>();
}

ComponentSystem::~ComponentSystem()
{

}

void ComponentSystem::RegisterComponent(int type, ComponentBase* component)
{

	// Observer를 상속받은 클래스인지 내부에서 확인한다
	m_pResourceObserverOwner->RegisterObserver(component);
	

	///2d vector ver
	///type번째 vector에 삽입한다.
	/// render component는 따로 관리
	if (type == static_cast<int>(ComponentType::Rendering))
	{
		m_RenderComponent_V.push_back(component);
	}
	/// 테스트용
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
	/// 랜더컴포넌트 벡터에 피직스도 들어가있으니 삭제해 줍니다. 임시방편
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
	/// auto쓸때 레퍼런스 사용을 검토해볼것, 수종
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
				//스타트 시점에서 자신의 오브젝트가 없는 컴포넌트는 시스템에서 제외된다.
				CA_TRACE("Component System : %d No Object", _pairVIter->second->GetComponetId());
				_componentV.erase(_pairVIter);
			}
		}
	}

}

void ComponentSystem::Update(float dTime)
{
	//컴포넌트 타입별 업데이트 순서를 제어해본다.
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
	// 라이트의 PreRender를 돌린다
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
