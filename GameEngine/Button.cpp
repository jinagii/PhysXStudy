#include "pch.h"
#include "Button.h"
#include "Sprite.h"
#include "EngineDLL.h"

#include "IRenderer.h"
#include "IResourceManager.h"

Button::Button()
	: ComponentBase(ComponentType::Rendering)
{
}

Button::~Button()
{

}

void Button::Start()
{
	// 키입력을 받아오기 위한 엔진
	m_pEngine = DLLEngine::GetEngine();

	// 그림을 그리기 위한 렌더러
	m_pRenderer = m_pEngine->GetIRenderer();

	m_pMyTransform = m_pMyObject->m_Transform;

	/// Observer를 통해 String을 Index로 가져와 그림을 그림
	/*
	for (int i = 0; i < 2; i++)
	{
		std::shared_ptr<SpriteData> _nowSpriteData(new SpriteData());
		m_pSpriteData_V.push_back(_nowSpriteData);
	}
	m_pSpriteData_V[(int)eBUTTON_STATE::PUT]->SetHide(true);

	for (int i = 0; i < m_pSpriteData_V.size(); ++i)
	{
		m_pRenderer->Draw2D(m_pMyObject->GetObjectFileName(i), m_pSpriteData_V[i], static_cast<int>(m_eRasterizer), m_worldTM);

		DirectX::SimpleMath::Vector3 _scale(m_pSpriteData_V[i]->width, m_pSpriteData_V[i]->height, m_pSpriteData_V[i]->depth);
		m_pMyTransform->SetScale(_scale);
	}
	*/

	m_worldTM = XMMatrixScaling(m_pMyTransform->m_Scale.x, m_pMyTransform->m_Scale.y, 1.0f)
		* XMMatrixRotationRollPitchYaw(m_pMyTransform->m_Rotation.x, m_pMyTransform->m_Rotation.y, m_pMyTransform->m_Rotation.z)
		* XMMatrixTranslation(m_pMyTransform->m_Position.x + m_pMyTransform->m_Scale.x / 2.0f, m_pMyTransform->m_Position.y + m_pMyTransform->m_Scale.y / 2.0f, m_pMyTransform->m_Position.z);

}

void Button::Update(float dTime)
{

}

void Button::OnRender()
{
	/*
	for (int i = 0; i < m_pSpriteData_V.size(); ++i)
	{
		m_pRenderer->Draw2D(m_pMyObject->GetObjectFileName(i), m_pSpriteData_V[i], static_cast<int>(m_eRasterizer), m_worldTM);

		DirectX::SimpleMath::Vector3 _scale(m_pSpriteData_V[i]->width, m_pSpriteData_V[i]->height, m_pSpriteData_V[i]->depth);
		m_pMyTransform->SetScale(_scale);
	}
	*/

	///////////////////////////////////////////////////


	m_NowState = eBUTTON_STATE::OFF;
	m_IsPushedBefore = m_IsPushed;
	m_IsPushed = false;

	//CA_TRACE("마우스위치 X : %d | Y : %d", m_pEngine->GetInput()->GetMouseClientPos().x, m_pEngine->GetInput()->GetMouseClientPos().y);
	//CA_TRACE("마우스 왼쪽 클릭 X : %d", m_pEngine->GetInput()->GetKey(VK_LBUTTON));
	if (Intersect(m_pEngine->GetInput()->GetMouseClientPos()))
	{
		m_NowState = eBUTTON_STATE::PUT;

		if (m_pEngine->GetInput()->GetKey(VK_LBUTTON))
		{
			m_IsPushed = true;
		}
	}

	/// 호버링
	//switch (m_NowState)
	//{
	//case eBUTTON_STATE::OFF:
	//	m_pSpriteData_V[(int)eBUTTON_STATE::OFF]->SetHide(false);
	//	m_pSpriteData_V[(int)eBUTTON_STATE::PUT]->SetHide(true);
	//	break;
	//case eBUTTON_STATE::PUT:
	//	m_pSpriteData_V[(int)eBUTTON_STATE::OFF]->SetHide(true);
	//	m_pSpriteData_V[(int)eBUTTON_STATE::PUT]->SetHide(false);
	//	break;
	//default:
	//	break;
	//}

	/*
	/// 누르고 떼고
	switch (m_IsPushed)
	{
	case false:
		m_pSpriteData_V[(int)eBUTTON_STATE::OFF]->SetHide(false);
		m_pSpriteData_V[(int)eBUTTON_STATE::PUT]->SetHide(true);
		break;
	case true:
		m_pSpriteData_V[(int)eBUTTON_STATE::OFF]->SetHide(true);
		m_pSpriteData_V[(int)eBUTTON_STATE::PUT]->SetHide(false);

		if (m_onClick && m_IsPushedBefore == false)
		{
			m_onClick();
		}
		break;
	default:
		break;
	}
	*/

	// 눌렸는지 아닌지 판단이 되면, 필요한 것만 그려준다
	switch (m_IsPushed)
	{
	case false:
		m_pRenderer->Draw2D(m_OffSpriteIndex, m_worldTM);
		break;
	case true:
		m_pRenderer->Draw2D(m_OnSpriteIndex, m_worldTM);
		if (m_onClick && m_IsPushedBefore == false)
		{
			m_onClick();
		}
		break;
	}
}

bool Button::GetIsPushed()
{
	return m_IsPushed;
}

bool Button::Intersect(POINT mousePos)
{
	RECT _nowRect = GetSpriteRect();

	// X축 체크
	if ((mousePos.x > _nowRect.left && _nowRect.right > mousePos.x) &&
		(mousePos.y > _nowRect.top && _nowRect.bottom > mousePos.y))
	{
		return true;
	}

	return false;
}

RECT Button::GetSpriteRect()
{
	RECT _newRect;

	_newRect.left = m_pMyTransform->m_Position.x;
	_newRect.top = m_pMyTransform->m_Position.y;
	_newRect.right = m_pMyTransform->m_Position.x + m_Rect.right;
	_newRect.bottom = m_pMyTransform->m_Position.y + m_Rect.bottom;

	return _newRect;
}

void Button::SetFunc(std::function<void()> onClick)
{
	m_onClick = onClick;
}


void Button::ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_OffSpriteIndex = pResourceManager->GetTextureIndex(m_OffSprite);
	m_OnSpriteIndex = pResourceManager->GetTextureIndex(m_OnSprite);

	m_Rect = pResourceManager->GetTextureRect(m_OffSpriteIndex);
}

void Button::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;
	
	m_SaveData->m_OffSprite = m_OffSprite;
	m_SaveData->m_OnSprite = m_OnSprite;
	
	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Button::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	
	SetEnable(m_SaveData->m_bEnable);
	
	m_OffSprite = m_SaveData->m_OffSprite;
	m_OnSprite = m_SaveData->m_OnSprite;
}

