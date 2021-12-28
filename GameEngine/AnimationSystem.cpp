#include "pch.h"

#include "AnimationSystem.h"
#include "IRenderer.h"
#include "MeshFilter.h"
#include "Transform.h"
#include "EngineDLL.h"
#include "IResourceManager.h"

#include "CATrace.h"

void Transition::AddParameter(const float standard, float* parameter, const bool greater)
{
	std::shared_ptr<Parameter> _newParam = std::make_shared<FloatType>(standard, parameter, greater);
	m_Parameter_V.push_back(_newParam);
}

void Transition::AddParameter(const bool* parameter, const bool standard)
{
	std::shared_ptr<Parameter>  _newParam = std::make_shared<BooleanType>(parameter, standard);
	m_Parameter_V.push_back(_newParam);
}

void Transition::TransitionUpdate()
{
	bool _nowResult = false;

	for (auto& _nowParam : m_Parameter_V)
	{
		if (_nowParam->ParameterCheck() == false)
		{
			return;
		}
	}

	// ������� �Դٸ� ��� true
	// ���� �����Ϳ� ���� �ڽ��� Ÿ�� �����͸� �Ѱ��ش�

	*m_pPrevState = *m_pCurrState;
	*m_pCurrState = m_TargetState.get();
	*m_pPrevTransition = this;

	State* _currState = *m_pCurrState;
	if (_currState != nullptr)
	{
		_currState->m_NowKeyFrame = 0;

		for (auto& _now : *_currState->m_Transition_V)
		{
			_now->m_IsCrossFadingOver = false;
		}
	}
}


State::State()
	: m_pCurrState(nullptr)
	, m_Speed(1.0f)
	, m_StateOffsetAngle(0.0f)
	, m_NowKeyFrame(0)
{
	m_Transition_V = new std::vector<std::shared_ptr<Transition>>();
}



void State::AddTrnasition(std::shared_ptr<State> state)
{
	std::shared_ptr<Transition> _newTransition = std::make_shared<Transition>(state, m_pCurrState, m_pPrevState, m_pPrevTransition);
	m_Transition_V->push_back(_newTransition);
}

_DLL void State::AddTrnasition(const std::string& stateName)
{
	std::shared_ptr<Transition> _newTransition =
		std::make_shared<Transition>(m_pMyAnimLayer->GetState(stateName), m_pCurrState, m_pPrevState, m_pPrevTransition);
	m_Transition_V->push_back(_newTransition);

	// ������Ʈ�� �� ��ŭ ũ�⸦ �ø��� �����Ѵ�.
	// ����ִ� ���Ͱ� ����� ���� �ƽ���
}


void State::SetAnimLayer(AnimLayer* pAnimLayer)
{
	m_pMyAnimLayer = pAnimLayer;
}

void State::Update()
{
	if (m_Transition_V->empty() == true) return;
	for (auto& _nowTransition : *m_Transition_V)
	{
		_nowTransition->TransitionUpdate();
	}
}

_DLL AnimLayer::AnimLayer()
	: m_bEnabled(true)
{
	m_pCurrState = new State * ();
	m_pPrevState = new State * ();
	m_pPrevTransition = new Transition * ();
}

_DLL AnimLayer::~AnimLayer()
{

}



_DLL void AnimLayer::AddStateMap(std::string name, const std::string& animationClipName)
{
	std::shared_ptr<State> _newState = std::make_shared<State>(m_pCurrState, m_pPrevState, m_pPrevTransition);
	_newState->m_StateName = name;
	_newState->m_AnimationName = animationClipName;
	_newState->SetAnimLayer(this);

	m_State_UM.insert({ name, _newState });
	m_States_V.push_back(_newState);
}

_DLL State* AnimLayer::GetNowState()
{
	if (*m_pCurrState != nullptr)
	{
		return *m_pCurrState;
	}
	else
	{
		return nullptr;
	}
}

std::vector<std::shared_ptr<State>>& AnimLayer::GetStates()
{
	return m_States_V;
}

_DLL std::shared_ptr<State> AnimLayer::GetState(const std::string& name)
{
	return m_State_UM.at(name);
}

void AnimLayer::LayerStart()
{
	*m_pCurrState = m_States_V.front().get();
}

void AnimLayer::LayerUpdate(GameObject* pGameObject, MeshFilter* pMeshFilter, std::shared_ptr<IRenderer> pRenderer)
{
	// Modified by YoKing
	// ��Ȱ��ȭ ���̾�� ������Ʈ ���� �ʴ´�.
	if (m_bEnabled == false)
	{
		return;
	}


	State* _pCurrState = nullptr;
	State* _pPrevState = nullptr;
	Transition* _pNowTransition = nullptr;

	if (m_States_V.size() == 1)
	{
		// ������Ʈ�� �ܼ��� ��
		_pCurrState = *m_pCurrState;
	}
	else
	{
		_pCurrState = *m_pCurrState;
		_pPrevState = *m_pPrevState;
		_pNowTransition = *m_pPrevTransition;
	}

	_pCurrState->Update();
	_pCurrState->m_NowKeyFrame++;

	switch (m_MaskingType)
	{
	case eMASKING_TYPE::NONE:
	{
		if (_pPrevState == nullptr)
		{
			pRenderer->AnimationProcess(
				pMeshFilter->GetModelMeshIndex(),
				_pCurrState->m_AnimationIndex,
				_pCurrState->m_NowKeyFrame,
				pGameObject->m_Transform->GetWorld(),
				_pCurrState->m_StateOffsetAngle);
			break;
		}
		
		if (_pNowTransition->m_FadingPeriod > 0)
		{
			if (_pNowTransition->m_IsCrossFadingOver == false)
			{
				_pNowTransition->m_IsCrossFadingOver = pRenderer->AnimationCrossFading(
					pMeshFilter->GetModelMeshIndex(),
					(*m_pCurrState)->m_AnimationIndex, (*m_pPrevState)->m_AnimationIndex,
					(*m_pCurrState)->m_NowKeyFrame, (*m_pPrevState)->m_NowKeyFrame,		// PrevState�� Ű�������� ��ġ ����
					_pNowTransition->m_FadingPeriod,
					pGameObject->m_Transform->GetWorld(),
					_pCurrState->m_StateOffsetAngle);

				pRenderer->EndAnimation(pMeshFilter->GetModelMeshIndex());

				return;
			}
		}


		pRenderer->AnimationProcess(
			pMeshFilter->GetModelMeshIndex(),
			_pCurrState->m_AnimationIndex,
			_pCurrState->m_NowKeyFrame,
			pGameObject->m_Transform->GetWorld(),
			_pCurrState->m_StateOffsetAngle);
		break;
	}
	case eMASKING_TYPE::OVERRINDING:
	{
		pRenderer->MaskingAnimation(
			pMeshFilter->GetModelMeshIndex(),
			_pCurrState->m_AnimationIndex,
			_pCurrState->m_NowKeyFrame,
			pGameObject->m_Transform->GetWorld());
		break;
	}
	}

	pRenderer->EndAnimation(pMeshFilter->GetModelMeshIndex());

}

// Modified by YoKing
_DLL void AnimLayer::SetEnabled(const bool val)
{
	m_bEnabled = val;
}


Animator::Animator()
{

}

Animator::~Animator()
{

}

void Animator::Start()
{
	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_MyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();

	// ��� ���̾���� �̸� ������ �Ǿ��־����
	for (auto& _nowAnimLayer : m_AnimLayer_V)
	{
		_nowAnimLayer->LayerStart();
	}
}

void Animator::Update(float dTime)
{
	if (TimeCheck(dTime))
	{
		// Ÿ�� üũ�� ����ؾ߸� ���̾� ��ü�� ������Ʈ�ȴ�
		for (auto& _nowAnimLayer : m_AnimLayer_V)
		{
			_nowAnimLayer->LayerUpdate(m_pMyObject, m_MyMeshFilter, m_pIRenderer);
		}
		m_TimeStack = 0.0f;
	}
}

void Animator::OnRender()
{

}

_DLL void Animator::AddAnimLayer(std::string name)
{
	std::shared_ptr<AnimLayer> _newLayer = std::make_shared<AnimLayer>();
	_newLayer->m_LayerName = name;

	m_AnimLayer_UM.insert({ name, _newLayer });
	m_AnimLayer_V.push_back(_newLayer);
}

_DLL std::shared_ptr<AnimLayer> Animator::GetAnimLayer(const std::string& name)
{
	// Modified by YoKing
	// �ٷ� UM ���� ���� �����ϴٰ��� �߸��� string �� ������ �� ���α׷��� �ٷ� ���������� ������ ���� ����..(out of range)
	// ���� find�� ã�Ƽ� ������ nullptr �����ϰ� caller �ʿ��� ����ó���� ����.
	if (m_AnimLayer_UM.find(name) == m_AnimLayer_UM.end())
	{
		return nullptr;
	}

	// ���������� ã���� �ش� value ����
	return m_AnimLayer_UM.at(name);
}

_DLL std::vector<std::shared_ptr<AnimLayer>>& Animator::GetAnimLayer()
{
	return m_AnimLayer_V;
}


void Animator::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	for (auto& _nowAnimLayer : m_AnimLayer_V)
	{
		for (auto& _pNowState : _nowAnimLayer->GetStates())
		{
			_pNowState->m_AnimationIndex = pReosureManager->GetAnimationClipDataIdnex(_pNowState->m_AnimationName);
		}
	}
}

