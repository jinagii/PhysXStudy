#pragma once
#include "PhysicsGeometry.h"

#include "ComponentBase.h"

enum RigidType
{
   /// 타입이 0이면 귀찮아짐
   Dynamic = 1, // 물리량이 가변적?
   Static // Transform을 바꾸지않는한 움직이지않는 static 강체
};

enum CollisionState;
__interface IRenderer;
struct CollisionHit;

/// <summary>
/// 이거 무조건 있어야합니다!
/// 
/// 2021.12.28 지나기
/// </summary>
class PhysicsActor : public ComponentBase
{
public:
   PhysicsActor();
   _DLL PhysicsActor(float radius, int type); // for sphere shape
   _DLL PhysicsActor(DirectX::SimpleMath::Vector3 box, int type); // for box shape
   _DLL PhysicsActor(float radius, float height, int type); // for capsule shape
   _DLL virtual ~PhysicsActor();

   virtual void Start() override;
   virtual void Update(float dTime) override;
   virtual void OnRender() override;

   // 단지 transform을 바꾸는 것이나 이를 물리엔진에서 액터의 PxTransform에 대입시켜주면 되나?
   void SetPhysicalTransform(Transform* transform);

   RigidType GetRigidType() const;
   _DLL GeometryBase* GetGeometry() const;

   std::shared_ptr<IRenderer> m_pIRenderer;

   int GetUserIndex() const;
   void SetUserIndex(int val);

   std::vector<PhysicsActor*> m_CollidedActor_V;

   //vector<CollisionHit> m_PrevCollisionHit_V;
   std::vector<CollisionHit> m_CurrCollisionHit_V;
private:
	// 생성된 액터에 접근할 인덱스
   unsigned int m_UserIndex;
	unsigned int m_PhysActorIndex;



   RigidType m_RigidType; // 강체 타입
   GeometryBase* m_pGeometry; // 액터의 형상, 모양



   DirectX::SimpleMath::Vector3 m_Velcity;
};
