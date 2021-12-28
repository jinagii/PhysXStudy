#include "pch.h"
#include "Animation.h"

#include "EngineDLL.h"
#include "IRenderer.h"
#include "IResourceManager.h"


Animation::Animation()
	: ComponentBase(ComponentType::Animation)
	, m_AnimationFrameRate(eANIM_FRAMERATE::FPS60)
	, m_NowKeyFrame(0)
	, m_OffsetAnlge(0.0f)
{

}


Animation::~Animation()
{

}

void Animation::Start()
{
	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_MyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();
}



void Animation::Update(float dTime)
{
	if (TimeCheck(dTime))
	{
		m_NowKeyFrame++;
		m_TimeStack = 0.0f;
	}

	// �ִϸ��̼� Ű�������� �Ѱ��ش�
	// ���� �Ѿ�� ���� ���ο��� firstKeyFrame���� �ٲ㼭 ��ȯ�Ѵ�
	// AnimationClipData�� ����� ���� �̷������� ����ߴ�
	m_pIRenderer->AnimationProcess(
		m_MyMeshFilter->GetModelMeshIndex(),
		m_AnimationClipIndex,
		m_NowKeyFrame,
		m_pMyObject->m_Transform->GetWorld(),
		m_OffsetAnlge);
}

void Animation::OnRender()
{

}

bool Animation::TimeCheck(float dTime)
{
	m_TimeStack += dTime;

	switch (m_AnimationFrameRate)
	{
	case eANIM_FRAMERATE::FPS60:
		return (m_TimeStack >= FRAME_60);
	case eANIM_FRAMERATE::FPS30:
		return (m_TimeStack >= FRAME_30);
	case eANIM_FRAMERATE::FPS24:
		return (m_TimeStack >= FRAME_24);
	default:
		break;
	}

	return false;
}

void Animation::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	m_AnimationClipIndex = pReosureManager->GetAnimationClipDataIdnex(m_AnimationClipName);
}
