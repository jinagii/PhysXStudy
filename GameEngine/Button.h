#pragma once

#include "DLLDefine.h"
#include "EngineDLL.h"

#include "ComponentBase.h"
#include "MeshRenderer.h"
#include "ResourceObserver.h"

__interface IRenderer;

class Camera;
class MeshFilter;
class MaterialComponent;
class Transform;
class Engine;
struct SpriteData;
struct Button_Save;

enum class eBUTTON_STATE
{
	OFF = 0,
	PUT,
};

class Button : public ComponentBase, public ResourceObserver
{
public:
	_DLL Button();
	_DLL virtual ~Button();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	bool GetIsPushed();
	_DLL void SetFunc(std::function<void()> onClick);

private:
	bool Intersect(POINT mousePos);
	RECT GetSpriteRect();

public:
	std::string m_OffSprite;
	std::string m_OnSprite;

private:
	Engine* m_pEngine;
	std::shared_ptr<IRenderer> m_pRenderer;
	
	eRasterizerState m_eRasterizer;

	Transform* m_pMyTransform;
	XMMATRIX m_worldTM;
	
	//std::vector<std::shared_ptr<SpriteData>> m_pSpriteData_V;
	unsigned int m_OffSpriteIndex;
	unsigned int m_OnSpriteIndex;

	RECT m_Rect;

	eBUTTON_STATE m_NowState = eBUTTON_STATE::OFF;
	bool m_IsPushedBefore = false;
	bool m_IsPushed = false;

	std::function<void()> m_onClick;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	Button_Save* m_SaveData;

};

struct Button_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;

	std::string m_OnSprite;
	std::string m_OffSprite;

};


BOOST_DESCRIBE_STRUCT(Button_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_OnSprite,
	m_OffSprite
	))

