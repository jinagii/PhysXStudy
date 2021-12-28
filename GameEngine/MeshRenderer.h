#pragma once


/// �������� �׸��� ��� �������� �����ؾ߰ڴ�
/// Debugging������ �ƴ���?

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

	// �����͸� �Ѱ��ִ� �۾����� �ʹ� �������� �����ִٺ��� ������� �����Ƿ� 
	// �ڵ����� ó�����ֱ� ���� Managers ��� Ŭ������ Ȱ���Ͽ� �ʱ�ȭ�Ѵ�.
	void Initialize();

	bool m_UseMaterial;

private:
	bool m_IsGizmoOn;
	eRasterizerState m_eRasterizer;
	eTopology m_eTopology;

	std::shared_ptr<IRenderer> m_pRenderer;
	Camera* m_pCamera;

	// ���� ������Ʈ�κ��� ������ ��
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