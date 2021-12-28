#pragma once


/// 렌더러가 그림을 어떻게 보여줄지 결정해야겠다
/// Debugging용인지 아닌지?

#include "DLLDefine.h"

#include "ComponentBase.h"

__interface IRenderer;

class Camera;
class MeshFilter;
class MaterialComponent;
class Transform;

struct MeshRenderer_Save;

enum class eRasterizerState
{
	SOLID = 0,
	WIREFRAME,
};

enum class eTopology
{
	TRIANGLELIST = 0,
	LINELIST
};

class MeshRenderer : public ComponentBase
{
public:
	_DLL MeshRenderer();
	_DLL virtual ~MeshRenderer();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	
	virtual void OnPreRender() override;
	virtual void OnRender() override;
	virtual void OnPostRender() override;

	_DLL void SetRasterizerState(eRasterizerState state);
	_DLL void SetGizmo(bool val);

	const bool& GetIsWireframeMode();
	const bool& GetIsGizmoOn();

	// 포인터를 넘겨주는 작업등이 너무 여기저기 펼쳐있다보면 깔끔하지 않으므로 
	// 자동으로 처리해주기 위해 Managers 라는 클래스를 활용하여 초기화한다.
	void Initialize();

	bool m_UseMaterial;

private:
	bool m_IsGizmoOn;
	eRasterizerState m_eRasterizer;
	eTopology m_eTopology;

	std::shared_ptr<IRenderer> m_pRenderer;
	Camera* m_pCamera;

	// 게임 오브젝트로부터 얻어오는 것
	Transform* m_pMyTransform;
	MeshFilter* m_pMyMeshFilter;
	MaterialComponent* m_pMyMaterial;

	bool m_CastShadow;
	
public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	MeshRenderer_Save* m_SaveData;
};


struct MeshRenderer_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;

	bool			m_IsGizmoOn;
	int				m_eRasterizer;
	int				m_eTopology;
	bool			m_CastShadow;
};


BOOST_DESCRIBE_STRUCT(MeshRenderer_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_IsGizmoOn,
	m_eRasterizer,
	m_eTopology,
	m_CastShadow
	))