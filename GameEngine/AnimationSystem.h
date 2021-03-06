#pragma once

/// FBX의 애니메이션 정보는 Clip으로 나뉜다.
/// Clip은 State에 들어가며, State에 따라 어떤 Clip이 재생될지 달라진다

#include "DLLDefine.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Animation.h"
#include "ResourceObserver.h"

__interface IRenderer;
class MeshFilter;
class Transition;
class Animator;
class AnimLayer;
class State;

class Parameter
{
public:
	virtual bool ParameterCheck() abstract;
};

class BooleanType : public Parameter
{
public:
	// Modified by YoKing
	BooleanType(const bool* parameter, const bool standard = true) : m_Parameter(parameter), m_Standard(standard) {}
	const bool* m_Parameter;	// 현재 원본의 값 
	const bool m_Standard;		// 결과 판단 기준	 
	bool ParameterCheck() override
	{
		return (*m_Parameter == m_Standard) ? true : false;
	}
};

class FloatType : public Parameter
{
public:
	FloatType(const float standard, float* parameter, const bool greater) :
		m_Standard(standard), m_Parameter(parameter), m_IsGreater(greater) {}

	const float m_Standard;		// 결과 값 판단 기준(처음에 정해지면 바뀌지 않음)
	float* m_Parameter;	// 클라이언트가 넘겨줄 값(계속해서 변화함)
	const bool m_IsGreater;

	bool ParameterCheck() override
	{
		// 큰 쪽으로 판단
		if (m_IsGreater == true)
		{
			return m_Standard < *m_Parameter ? true : false;
		}
		else
		{
			return m_Standard > *m_Parameter ? true : false;
		}
	}
};

class Transition
{
public:
	Transition(std::shared_ptr<State> target
		, State** pCurrState, State** pPrevState, Transition** pPrevTransition)
		: m_pCurrState(pCurrState), m_pPrevState(pPrevState), m_pPrevTransition(pPrevTransition)
		, m_TargetState(target)
		, m_FadingPeriod(0) {};
	_DLL void AddParameter(const float standard, float* parameter, const bool greater);
	_DLL void AddParameter(const bool* parameter, const bool standard = true);

	void TransitionUpdate();


	// 전체 애니메이션의 길이에 몇 퍼센트인지
	unsigned int m_FadingPeriod;

	bool m_IsCrossFadingOver;

private:
	std::vector< std::shared_ptr<Parameter>> m_Parameter_V;
	const std::shared_ptr<State> m_TargetState;
	State** m_pCurrState;
	State** m_pPrevState;
	Transition** m_pPrevTransition;
};


class State
{
public:
	State();
	State(State** pCurrState, State** pPrevState, Transition** pPrevTransition)
		: m_pCurrState(pCurrState), m_pPrevState(pPrevState), m_pPrevTransition(pPrevTransition)
		, m_AnimationIndex(0)
		, m_Speed(1.0f)
		, m_StateOffsetAngle(0.0f)
		, m_NowKeyFrame(0)
	{
		m_Transition_V = new std::vector<std::shared_ptr<Transition>>();
	}
	~State() {};

	_DLL void AddTrnasition(std::shared_ptr<State> state);
	_DLL void AddTrnasition(const std::string& stateName);

	void SetAnimLayer(AnimLayer* pAnimLayer);

	void Update();

	std::string m_StateName;
	unsigned int m_StateIndex;

	std::string m_AnimationName;
	unsigned int m_AnimationIndex;

	// State가 업데이트될 때마다 들어와서 체크함
	std::vector<std::shared_ptr<Transition>>* m_Transition_V;
	AnimLayer* m_pMyAnimLayer;	// 기존 애니메이터의 역할

public:
	unsigned int m_NowKeyFrame;		// 중간에 키프레임이 바뀌면 스테이트가 들고 있는다
	float m_Speed;
	float m_StateOffsetAngle;
	bool m_Mirror;					// 좌우반전해서 애니메이션을 보여줄 것인가? 아직 어떻게할지 모름

private:
	// 대표 스테이트, 애니메이터에게 현재 스테이트를 알려줄 용도
	State** m_pCurrState;
	State** m_pPrevState;
	Transition** m_pPrevTransition;
};

enum class eMASKING_TYPE
{
	NONE = 0,
	OVERRINDING,
};

class AnimLayer
{
public:
	_DLL AnimLayer();
	_DLL virtual ~AnimLayer();

	_DLL void AddStateMap(std::string name, const std::string& animationClipName);
	_DLL State* GetNowState();

	// 컴포넌트 시스템이 스테이트에 접근해서 애니메이션 인덱스를 전달하기 위해
	std::vector<std::shared_ptr<State>>& GetStates();
	_DLL std::shared_ptr<State> GetState(const std::string& name);

	std::string m_LayerName;
	void LayerStart();
	void LayerUpdate(GameObject* pGameObject, MeshFilter* pMeshFilter, std::shared_ptr<IRenderer> pRenderer);

	// 애니메이터는 다수의 State를 가지고 있다
	std::vector<std::shared_ptr<State>> m_States_V;
	// 스테이트의 이름(Idle, Run 등)
	std::unordered_map<std::string, std::shared_ptr<State>> m_State_UM;

	eMASKING_TYPE m_MaskingType;

	// Modified by YoKing
	_DLL void SetEnabled(const bool val);

	bool m_bEnabled;	// 레이어 On/Off 상태 True면 적용(업데이트)

private:
	// 지금 도달해있는 state가 누구인지를 알려줄 방법이 필요
	State** m_pCurrState;
	State** m_pPrevState;
	Transition** m_pPrevTransition;

};


class Animator : public Animation
{
public:
	_DLL Animator();
	_DLL virtual ~Animator();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL void AddAnimLayer(std::string name);
	_DLL std::shared_ptr<AnimLayer> GetAnimLayer(const std::string& name);
	_DLL std::vector<std::shared_ptr<AnimLayer>>& GetAnimLayer();

	std::vector<std::shared_ptr<AnimLayer>> m_AnimLayer_V;
	std::unordered_map<std::string, std::shared_ptr<AnimLayer>> m_AnimLayer_UM;


public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

	

};