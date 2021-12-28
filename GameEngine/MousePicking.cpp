#include "pch.h"
#include "Scene.h"
#include "MousePicking.h"

MousePicking::MousePicking(Scene* pScene)
{

}

MousePicking::~MousePicking()
{

}

bool MousePicking::IsPicked() const
{
   return true;
}

bool MousePicking::Pick()
{
   return true;

}

void MousePicking::GrabActor(DirectX::SimpleMath::Vector3& worldImpact, DirectX::SimpleMath::Vector3& rayOrigin)
{
   //if(!m_pSelectedActor||m_pSelectedActor->get != )


}

void MousePicking::MoveActor(int x, int y)
{

}
