#pragma once

#include "DLLDefine.h"

//
//class PxTransform;
//class PxRigidDynamic;
//class PxGeometry;
//
//class PxDefaultAllocator;
//class PxDefaultErrorCallback;
//
//class PxFoundation;
//class PxDefaultCpuDispatcher;
//class PxScene;
//class PxPhysics;
//class PxMaterial;
//enum GeometryType;

class Transform;
class PhysicsActor;
class SceneBase;
class GameObject;
struct RaycastHit;
struct CollisionHit;
class GeometryBase;

/// PhysX�� ���ΰ� ���� loop�� �����ϱ����� Ŭ����
/// �츮�� mesh��� ����� ����� �ϱ����� ����ؾ�����...?
///

/// PhysX�� 
class PhysicsEngine
{
public:
   PhysicsEngine();
   ~PhysicsEngine();

   void Initialize(bool isInteractive = true);
   void PhysicsLoop(bool interactive = true);
   void ReleasePhysics(bool interactive = true);
   void ReleasePhysicsActor(bool interactive = true);

   void CreateTestBox(Transform* transform, float size);
   void CreateDynamicObject(Transform* transform, int geometry, DirectX::SimpleMath::Vector3 velocity);
   void CreateStack(Transform* transform, uint32_t size, float halfExtent);

   void AddPhysicsScene(SceneBase* scene);
   void SetNowPhysicsScene(std::string name);
   void AddPhysicsActor(PhysicsActor* object); // ���͸� �߰�, �������� �������ְ� �־���

   void SetPhysicsTransform(Transform* transform);
   void UpdatePhysicsTransform(PhysicsActor* actor);
   void SetTransform(GameObject* object, const Transform& pos);
   void SetVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel);
   void SetAngularVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel);

   GameObject* GetGameObjectByIndex(unsigned int index);
   PhysicsActor* GetPhysicsActorByIndex(unsigned int index);
   unsigned int GetIndexFromGameObject(GameObject* object);
   unsigned int GetIndexFromPhysicsActor(PhysicsActor* actor);

   //_DLL GameObject* RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
   //                                        DirectX::SimpleMath::Vector3 rayDir,
   //                                        GeometryBase* geometry, Transform* transform,
   //                                        float maxDist, int hitFlag);

   _DLL GameObject* RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin,
                                           const DirectX::SimpleMath::Vector3& dir, float dist);
   _DLL RaycastHit CheckRaycastHit(const DirectX::SimpleMath::Vector3& origin,
                                   const DirectX::SimpleMath::Vector3& dir, float dist);
   _DLL void CheckCollision(std::vector<CollisionHit>* data, PhysicsActor* actor, Transform* transform);
   void CheckCollision2(std::vector<CollisionHit>* data, PhysicsActor* actor, Transform* transform);

   _DLL bool MousePicking(int x, int y);


   /// how to replace to our renderer??

   std::vector<PhysicsActor*> m_PhysicsActors_V;
   std::unordered_map<int, PhysicsActor*> m_PhysicsActors_UM;

private:
   POINT m_MousePos;



};
