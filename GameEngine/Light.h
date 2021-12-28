#pragma once

#include "DLLDefine.h"
#include "ComponentBase.h"

enum class eLIGHT_TYPE
{
	DIRECTIONAL = 0,
	POINT,
	SPOT,
};

__interface IRenderer;
class Transform;
struct Light_Save;

class Light : public ComponentBase
{
public:

	_DLL Light();
	_DLL virtual ~Light();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;

	virtual void OnPreRender() override;
	virtual void OnRender() override;


	_DLL void SetPostion(const DirectX::SimpleMath::Vector3& pos);
	_DLL void SetRotation(const DirectX::SimpleMath::Vector3& axis, float rotVal);
	_DLL void SetColor(const DirectX::SimpleMath::Vector3& color);
	_DLL void SetColor(const float& r, const float& g, const float& b);
	_DLL void SetIntensity(float val);


	_DLL const DirectX::SimpleMath::Matrix& GetTransform();
	_DLL const DirectX::SimpleMath::Vector3& GetPostion();
	_DLL const DirectX::SimpleMath::Quaternion& GetRotation();
	_DLL const DirectX::SimpleMath::Vector3& GetColor();
	_DLL float GetIntensity();

	const eLIGHT_TYPE& GetLightType();


	// 일부러 퍼블릭으로 뻈음
	eLIGHT_TYPE m_eLightType;

	bool m_CastShadow;

private:
	std::shared_ptr<IRenderer> m_pRenderer;

	Transform* m_pMyTransform;

	DirectX::SimpleMath::Vector3 m_Color;
	float m_Intensity;

public:
	float m_FalloffStart;
	float m_FalloffEnd;
	float m_SpotPower;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	Light_Save* m_SaveData;
};

struct Light_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;
	int		m_eLightType;
	bool	m_CastShadow;
	float	m_Intensity;
	float	m_Color[3];
	float	m_FalloffStart;
	float	m_FalloffEnd;
	float	m_SpotPower;
	//float* m_Test_V;
	//unsigned int m_Test_S;
};

BOOST_DESCRIBE_STRUCT(Light_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_eLightType,
	m_CastShadow,
	m_Intensity,
	m_Color,
	m_FalloffStart,
	m_FalloffEnd,
	m_SpotPower
	//m_Test_V,
	//m_Test_S
	))