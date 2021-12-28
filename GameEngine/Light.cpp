#include "pch.h"
#include "Light.h"
#include "Transform.h"

#include "IRenderer.h"
#include "EngineDLL.h"
#include "Managers.h"

Light::Light()
	: ComponentBase(ComponentType::Light)
	, m_eLightType(eLIGHT_TYPE::DIRECTIONAL)
	, m_Intensity(1.0f)
	, m_FalloffStart(0.0f)
	, m_FalloffEnd(0.0f)
	, m_SpotPower(0.0f)
	, m_CastShadow(false)
	, m_SaveData(new Light_Save())
{

}

Light::~Light()
{

}

void Light::Start()
{
	m_pMyTransform = m_pMyObject->GetComponent<Transform>();
	m_Color = { 1.0f, 1.0f, 1.0f };

	Engine* engine = DLLEngine::GetEngine();
	m_pRenderer = engine->GetIRenderer();

}

void Light::Update(float dTime)
{


}

void Light::OnPreRender()
{

	DirectX::XMFLOAT3 _nowAnlge;

	_nowAnlge.x = m_pMyTransform->m_EulerAngles.x * MathHelper::Pi / 180;
	_nowAnlge.y = m_pMyTransform->m_EulerAngles.y * MathHelper::Pi / 180;
	_nowAnlge.z = m_pMyTransform->m_EulerAngles.z * MathHelper::Pi / 180;

	m_pRenderer->LightUpdate(
		static_cast<unsigned int>(m_eLightType),
		m_pMyTransform->GetWorldPosition(), _nowAnlge, m_Color,
		m_Intensity, m_FalloffStart, m_FalloffEnd, m_SpotPower, m_CastShadow);


	if (m_CastShadow == true)
	{
		m_pRenderer->BeginShadow();
		Managers::GetInstance()->GetComponentSystem()->LightPreRender();
	}

}

void Light::OnRender()
{


}


_DLL void Light::SetPostion(const DirectX::SimpleMath::Vector3& pos)
{
	m_pMyTransform->SetPosition(pos);
}

_DLL void Light::SetRotation(const DirectX::SimpleMath::Vector3& axis, float rotVal)
{
	m_pMyTransform->SetRotation(DirectX::SimpleMath::Quaternion(axis, rotVal));
}

_DLL void Light::SetColor(const DirectX::SimpleMath::Vector3& color)
{
	m_Color = color;
}

_DLL void Light::SetColor(const float& r, const float& g, const float& b)
{
	m_Color.x = r;
	m_Color.y = g;
	m_Color.z = b;
}

_DLL void Light::SetIntensity(float val)
{
	m_Intensity = val;
}

const DirectX::SimpleMath::Matrix& Light::GetTransform()
{
	return m_pMyTransform->GetWorld();
}

const DirectX::SimpleMath::Vector3& Light::GetPostion()
{
	return m_pMyTransform->GetLocalPosition();
}

const DirectX::SimpleMath::Quaternion& Light::GetRotation()
{
	return m_pMyTransform->GetRotation();
}

const DirectX::SimpleMath::Vector3& Light::GetColor()
{
	return m_Color;
}

float Light::GetIntensity()
{
	return m_Intensity;
}

const eLIGHT_TYPE& Light::GetLightType()
{
	return m_eLightType;
}

void Light::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_eLightType = static_cast<int>(m_eLightType);
	m_SaveData->m_CastShadow = m_CastShadow;
	m_SaveData->m_Intensity = m_Intensity;
	m_SaveData->m_Color[0] = m_Color.x;
	m_SaveData->m_Color[1] = m_Color.y;
	m_SaveData->m_Color[2] = m_Color.z;
	m_SaveData->m_FalloffStart = m_FalloffStart;
	m_SaveData->m_FalloffEnd = m_FalloffEnd;
	m_SaveData->m_SpotPower = m_SpotPower;

	//std::vector<float>* a = new std::vector<float>;
	//a->push_back(0.0f);
	//a->push_back(1.0f);
	//a->push_back(2.0f);
	//m_SaveData->m_Test_V = a->data();
	//m_SaveData->m_Test_S = a->size();

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Light::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);

	m_eLightType = static_cast<decltype(m_eLightType)>(m_SaveData->m_eLightType);
	m_CastShadow = m_SaveData->m_CastShadow;
	m_Intensity = m_SaveData->m_Intensity;
	m_Color.x = m_SaveData->m_Color[0];
	m_Color.y = m_SaveData->m_Color[1];
	m_Color.z = m_SaveData->m_Color[2];
	m_FalloffStart = m_SaveData->m_FalloffStart;
	m_FalloffEnd = m_SaveData->m_FalloffEnd;
	m_SpotPower = m_SaveData->m_SpotPower;
}
