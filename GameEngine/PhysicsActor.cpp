#include "pch.h"
#include "Transform.h"
#include "Raycast.h"
#include "PhysicsActor.h"
#include "PhysicsUtility.h"
#include "Input.h"

#include "EngineDLL.h"
#include "IRenderer.h"

// 같은 번호는 있으면 안됨, 지면은 0임
static int g_UserIndex = 0;

/// <summary>
/// 생성시에 Geo나 타입을 정함
/// </summary>
PhysicsActor::PhysicsActor() : ComponentBase(ComponentType::Physics)
{
   m_pGeometry = new SphereGeometry(5.0f);
   g_UserIndex++;
   m_UserIndex = g_UserIndex;
   m_RigidType = RigidType::Static;
}

PhysicsActor::PhysicsActor(float radius, int type) : ComponentBase(ComponentType::Physics)
{
   m_pGeometry = new SphereGeometry(radius);
   g_UserIndex++;
   m_UserIndex = g_UserIndex;
   m_RigidType = (RigidType)type;
}

PhysicsActor::PhysicsActor(DirectX::SimpleMath::Vector3 box, int type) : ComponentBase(ComponentType::Physics)
{
   m_pGeometry = new BoxGeometry(box.x, box.y, box.z);
   g_UserIndex++;
   m_UserIndex = g_UserIndex;
   m_RigidType = (RigidType)type;
}

PhysicsActor::PhysicsActor(float radius, float height, int type) : ComponentBase(ComponentType::Physics)
{
   m_pGeometry = new CapsuleGeometry(radius, height);
   g_UserIndex++;
   m_UserIndex = g_UserIndex;
   m_RigidType = (RigidType)type;
}

PhysicsActor::~PhysicsActor()
{

}

/// <summary>
/// 시작 시 타입을 찾아 디버깅 렌더러를 붙임
/// </summary>
void PhysicsActor::Start()
{
   m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();

   switch (m_pGeometry->GetType())
   {
      case Sphere:
      {
         SphereGeometry* _sphere = dynamic_cast<SphereGeometry*>(m_pGeometry);
         m_PhysActorIndex = m_pIRenderer->CreatePysicsActorSphere(_sphere->GetSphereRadius());
         break;
      }
      case Box:
      {
         BoxGeometry* _box = dynamic_cast<BoxGeometry*>(m_pGeometry);
         m_PhysActorIndex = m_pIRenderer->CreatePysicsActorBox(_box->GetBoxSize().x, _box->GetBoxSize().y, _box->GetBoxSize().z);
         break;
      }
      case Capsule:
      {
         CapsuleGeometry* _capsule = dynamic_cast<CapsuleGeometry*>(m_pGeometry);
         ///m_ActorIndex = m_pIRenderer->CreatePysicsActorBox(_capsule->GetCapsuleRadius(), _capsule->GetCapsuleHeight());
         break;
      }

      default:
      {
         SphereGeometry* _sphere = dynamic_cast<SphereGeometry*>(m_pGeometry);
         m_PhysActorIndex = m_pIRenderer->CreatePysicsActorSphere(_sphere->GetSphereRadius());
         break;
      }
   }


   // 액터들을 탐색하고 넣어주기?
   m_pMyObject->SetHasPhysics(true);

}

/// <summary>
///  굳이 쓸일 없을듯
/// </summary>
/// <param name="dTime"></param>
void PhysicsActor::Update(float dTime)
{
   // 액터들의 트랜스폼을 피직스공간으로 넣어주기?

   if (GetAsyncKeyState(VK_SPACE & 0x0001))
   {
      //m_pMyObject->m_Transform
   }

}

/// <summary>
/// 디버깅 렌더러를 그림
/// </summary>
void PhysicsActor::OnRender()
{
   /// 이후 수종 작업
   switch (m_pGeometry->GetType())
   {
      case Sphere:
      {
         m_pIRenderer->RenderSphere(m_PhysActorIndex, m_pMyObject->m_Transform->GetWorld());
         break;
      }
      case Box:
      {
         m_pIRenderer->RenderBox(m_PhysActorIndex, m_pMyObject->m_Transform->GetWorld());
         break;
      }
      case Capsule: /// 반지름과, 높이로 캡슐 그려야함!! 수종 화이팅
      {
         ///m_pIRenderer->RenderBox(m_ActorIndex, m_pMyObject->m_Transform->GetWorld());
         break;
      }
      default:
      {
         m_pIRenderer->RenderSphere(m_PhysActorIndex, m_pMyObject->m_Transform->GetWorld());

         break;
      }
   }

   ///m_pIRenderer->DrawDebugging()
}

void PhysicsActor::SetPhysicalTransform(Transform* transform)
{
   m_Transform = transform;
}

GeometryBase* PhysicsActor::GetGeometry() const
{
   return m_pGeometry; 
}

int PhysicsActor::GetUserIndex() const
{
   return m_UserIndex;
}

void PhysicsActor::SetUserIndex(int val)
{
   m_UserIndex = val;
}

RigidType PhysicsActor::GetRigidType() const
{
   return m_RigidType; 
}
