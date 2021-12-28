#pragma once
#include "PhysicsGeometry.h"

#include "ComponentBase.h"

enum RigidType
{
   /// Ÿ���� 0�̸� ��������
   Dynamic = 1, // �������� ������?
   Static // Transform�� �ٲ����ʴ��� ���������ʴ� static ��ü
};

enum CollisionState;
__interface IRenderer;
struct CollisionHit;

/// <summary>
/// �̰� ������ �־���մϴ�!
/// 
/// 2021.12.28 ������
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

   // ���� transform�� �ٲٴ� ���̳� �̸� ������������ ������ PxTransform�� ���Խ����ָ� �ǳ�?
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
	// ������ ���Ϳ� ������ �ε���
   unsigned int m_UserIndex;
	unsigned int m_PhysActorIndex;



   RigidType m_RigidType; // ��ü Ÿ��
   GeometryBase* m_pGeometry; // ������ ����, ���



   DirectX::SimpleMath::Vector3 m_Velcity;
};
