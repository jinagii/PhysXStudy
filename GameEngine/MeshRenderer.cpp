#include "pch.h"
#include "MeshRenderer.h"

#include "IRenderer.h"

#include "Camera.h"
#include "Transform.h"
#include "MeshFilter.h"
#include "MaterialComponent.h" 
#include "EngineDLL.h"

#include "CATrace.h"

MeshRenderer::MeshRenderer() :
	ComponentBase(ComponentType::Rendering),
	m_IsGizmoOn(true),
	m_pMyMeshFilter(nullptr),
	m_pMyMaterial(nullptr),
	m_eRasterizer(eRasterizerState::SOLID),
	m_eTopology(eTopology::TRIANGLELIST),
	m_CastShadow(true),
	m_SaveData(new MeshRenderer_Save())
{

}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::Start()
{
	m_pMyTransform = m_pMyObject->GetComponent<Transform>();
	m_pMyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();
	m_pMyMaterial = m_pMyObject->GetComponent<MaterialComponent>();


	Initialize();
}

void MeshRenderer::Update(float dTime)
{

}

void MeshRenderer::OnPreRender()
{
	if (m_CastShadow == true)
	{
		m_pRenderer->ShadowPath(m_pMyMeshFilter->GetModelMeshIndex(), m_pMyTransform->GetWorld());
	}
}

void MeshRenderer::OnRender()
{
	m_pRenderer->CameraUpdate(m_pCamera->m_Transform->GetWorldPosition(), m_pCamera->View(), m_pCamera->Proj());

	m_pRenderer->Draw(
		m_IsGizmoOn,
		static_cast<int>(m_eRasterizer),
		static_cast<int>(m_eTopology),
		m_pMyMeshFilter->GetCurrentIndex(),
		m_pMyTransform->GetWorld(), m_pCamera->View(), m_pCamera->Proj());

}

void MeshRenderer::OnPostRender()
{

}

_DLL void MeshRenderer::SetRasterizerState(eRasterizerState state)
{
	m_eRasterizer = state;
}

_DLL void MeshRenderer::SetGizmo(bool val)
{
	m_IsGizmoOn = val;
}

const bool& MeshRenderer::GetIsWireframeMode()
{
	return m_eRasterizer == eRasterizerState::WIREFRAME ? true : false;
}

const bool& MeshRenderer::GetIsGizmoOn()
{
	return m_IsGizmoOn;
}

void MeshRenderer::Initialize()
{
	Engine* engine = DLLEngine::GetEngine();
	m_pRenderer = engine->GetIRenderer();

	//m_pCamera = engine->GetCamera();
	m_pCamera = Managers::GetInstance()->GetCameraManager()->GetNowCamera();

}

void MeshRenderer::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_IsGizmoOn = m_IsGizmoOn;
	m_SaveData->m_eRasterizer = (int)m_eRasterizer;
	m_SaveData->m_eTopology = (int)m_eTopology;
	m_SaveData->m_CastShadow = m_CastShadow;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void MeshRenderer::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);

	m_IsGizmoOn = m_SaveData->m_IsGizmoOn;
	m_eRasterizer =static_cast<decltype(m_eRasterizer)>(m_SaveData->m_eRasterizer);
	m_eTopology = static_cast<decltype(m_eTopology)>(m_SaveData->m_eTopology);
	m_CastShadow = m_SaveData->m_CastShadow;
}

