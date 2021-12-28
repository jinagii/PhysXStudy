#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "SceneBase.h"
// SDK
//#include "PxPhysics.h"
//#include "PxPhysicsAPI.h"
// for wrapping
#include "PhysicsActor.h"
#include "PhysicsUtility.h"
#include "Raycast.h"
#include "PhysicsGeometry.h"
#include "CollisionCallBack.h"

#include "PhysicsEngine.h"

//////////////////////////////////////////////////////////////////////////

static physx::PxDefaultAllocator g_Allocator;
static physx::PxDefaultErrorCallback g_ErrorCallback;
static physx::PxDefaultCpuDispatcher* g_pDispatcher; // implementation to cpu
static physx::PxTolerancesScale g_ToleranceScale;
//static physx::PxPvd* g_pPVD; // for visual debugger

static physx::PxFoundation* g_pFoundation; // about settings (init, etc.)
static physx::PxPhysics* g_pPhysics; // core

static physx::PxScene* g_pNowScene;
static unordered_map<std::string, physx::PxScene*> g_pScene_UM;

static physx::PxMaterial* g_pMaterial;

//////////////////////////////////////////////////////////////////////////

PhysicsEngine::PhysicsEngine()
{
   //physx::PxRaycastHit _ray;
   //_ray.
}

PhysicsEngine::~PhysicsEngine()
{

}

/// <summary>
/// �̰Ŵ� Managers���� ����
/// </summary>
/// <param name="isInteractive"></param>
void PhysicsEngine::Initialize(bool isInteractive)
{
   g_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_Allocator, g_ErrorCallback);

   /// pvd
   //g_pPVD = physx::PxCreatePvd(*g_pFoundation);
   //physx::PxPvdTransport* _transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
   //g_pPVD->connect(*_transport, physx::PxPvdInstrumentationFlag::eALL);

   g_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_pFoundation, physx::PxTolerancesScale(), true /*, g_pPVD*/); // gPvd

   //physx::PxSceneDesc _sceneDesc(g_pPhysics->getTolerancesScale());
   //_sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // �߷°�
   //g_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
   //_sceneDesc.cpuDispatcher = g_pDispatcher;
   //_sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
   //g_pScene = g_pPhysics->createScene(_sceneDesc);

   //physx::PxPvdSceneClient* _pvdClient = g_pScene->getScenePvdClient();
   //if (_pvdClient)
   //{
   //   _pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
   //   _pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
   //   _pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
   //}

   //////////////////////////////////////////////////////////////////////////
   /// for Test scene
   // create simulation

   // ��Ƽ���� ���� staticFriction, dynamicFriction, restitution
   g_pMaterial = g_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

}

/// <summary>
/// �̰͵� Managers -> Engine���� ����
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::PhysicsLoop(bool interactive)
{
   g_pNowScene->simulate(1.0f / 60.0f);
   g_pNowScene->fetchResults(true); // ������ ���ǰ� �Ⱦ��ǰ�?

   for (PhysicsActor* _actor : m_PhysicsActors_V)
   {
      UpdatePhysicsTransform(_actor);
   }


}

/// <summary>
/// ������ ���� ������
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::ReleasePhysics(bool interactive)
{
   //Safe_Release(g_pScene);
   Safe_Release(g_pNowScene);
   Safe_Release(g_pDispatcher);
   Safe_Release(g_pPhysics);

   //if (g_pPVD)
   //{
   //   physx::PxPvdTransport* _transport = g_pPVD->getTransport();
   //   g_pPVD->release();	g_pPVD = NULL;
   //   Safe_Release(_transport);
   //}

   Safe_Release(g_pFoundation);
}

/// <summary>
/// �ַ� ����ȯ�Ҷ� ���͵� ����
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::ReleasePhysicsActor(bool interactive)
{
   // �ϴ� ��üŬ����


   m_PhysicsActors_V.clear();



   /// for������ ���� �ʿ�




}

/// <summary>
/// ��������� ��������� (�����), pvd������ �۵���
/// ���� ��Ÿ�� �߿� physicsActor�� �������� ����������� Ȯ���ϱ����� �������
/// </summary>
/// <param name="transform"></param>
/// <param name="size"></param>
void PhysicsEngine::CreateTestBox(Transform* transform, float size)
{
   physx::PxTransform _pxTr = CreatePxTransform(*transform);

   physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(size, size, size), *g_pMaterial);
   //physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(0, 1, 0, 0), *g_pMaterial); // ����
   physx::PxRigidStatic* _body = physx::PxCreateStatic(*g_pPhysics, _pxTr, *_shape);
   //physx::PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pxTr);

   _body->attachShape(*_shape);

   g_pNowScene->addActor(*_body);
}
/// <summary>
/// ����� (���� �Ⱦ�)
/// </summary>
/// <param name="transform"></param>
/// <param name="geometry"></param>
/// <param name="velocity"></param>
void PhysicsEngine::CreateDynamicObject(Transform* transform, int geometry, Vector3 velocity)
{
   physx::PxTransform _pxTr = CreatePxTransform(*transform);

   GeometryType _GeoType = static_cast<GeometryType>(geometry);

   physx::PxGeometry* _nowPxGeo;
   switch (_GeoType)
   {
      case Sphere:
      {
         _nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
         break;
      }
      case Box:
      {
         _nowPxGeo = CreateGeometry<physx::PxBoxGeometry>();
         break;
      }
      default:
      {
         _nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
         break;
      }
   }

   /// �Ʒ� �ڵ�� �ȸ�������� why?
   //physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*g_pPhysics, _pxTr, *_nowPxGeo, *g_pMaterial, 10.0f); 

   /// �Ʒ� �ڵ�� ���� �������
   physx::PxRigidDynamic* _dynamicObj = g_pPhysics->createRigidDynamic(_pxTr);
   physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *g_pMaterial);
   _dynamicObj->attachShape(*_shape);
   physx::PxRigidBodyExt::updateMassAndInertia(*_dynamicObj, 10.0f);

   physx::PxVec3 _vec(velocity.x, velocity.y, velocity.z);

   _dynamicObj->setAngularDamping(0.5f);
   _dynamicObj->setLinearVelocity(_vec);

   g_pNowScene->addActor(*_dynamicObj);
}

/// <summary>
/// ����� (���� �Ⱦ�)
/// </summary>
/// <param name="transform"></param>
/// <param name="size"></param>
/// <param name="halfExtent"></param>
void PhysicsEngine::CreateStack(Transform* transform, uint32_t size, float halfExtent)
{
   physx::PxTransform _pxTr = CreatePxTransform(*transform);

   physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *g_pMaterial);
   for (uint32_t i = 0; i < size; i++)
   {
      for (uint32_t j = 0; j < size - i; j++)
      {
         physx::PxTransform _localTm(physx::PxVec3(float(j * 2) - float(size - i), float(i * 2 + 1), 0) * halfExtent);
         physx::PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pxTr.transform(_localTm));
         _body->attachShape(*_shape);
         physx::PxRigidBodyExt::updateMassAndInertia(*_body, 10.0f);
         g_pNowScene->addActor(*_body);
      }
   }

   _shape->release();
}

/// <summary>
/// Managers���� ���� (���� isPhysicsScene�̸� �˾Ƽ� ���)
/// Tolerance�ϳ����ϸ� �״�� ��
/// </summary>
/// <param name="scene"></param>
void PhysicsEngine::AddPhysicsScene(SceneBase* scene)
{
   static bool _isFirstScene = true;

   physx::PxSceneDesc _sceneDesc(g_pPhysics->getTolerancesScale());
   _sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // �߷°�
   /// �̿밡���� CPU�� ������ ������ �������� �������Լ��� �Ẹ��
   g_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
   _sceneDesc.cpuDispatcher = g_pDispatcher;
   _sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; /// �����𸣰ڴ�...
   physx::PxScene* _pScene = g_pPhysics->createScene(_sceneDesc);

   // rigid ������ ����� �ش�
   physx::PxRigidStatic* _groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(0, 1, 0, 0), *g_pMaterial);
   /// ���鸸��°� Ȯ���غ���
   //physx::PxVec3 _p1(0.0f, 0.0f, 10.0f);
   //physx::PxVec3 _p2(-10.0f, 0.0f, -10.0f);
   //physx::PxVec3 _p3(10.0f, 0.0f, -10.0f);
   //physx::PxRigidStatic* _groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(_p1, _p2, _p3), *g_pMaterial);
   _pScene->addActor(*_groundPlane); // ������ ���� ������ �߰�

   g_pScene_UM.insert({ scene->GetSceneName(), _pScene });

   if (_isFirstScene == true)
   {
      SetNowPhysicsScene(scene->GetSceneName());
      _isFirstScene = false;
   }
}

/// <summary>
/// �˾Ƽ� �ǰԲ� �����ʿ�
/// </summary>
/// <param name="name"></param>
void PhysicsEngine::SetNowPhysicsScene(std::string name)
{
   unordered_map<string, physx::PxScene*>::iterator _scene = g_pScene_UM.find(name);

   if (_scene == g_pScene_UM.end())
   {
      Trace("Can not find the scene");
   }

   if (g_pNowScene != nullptr)
   {
      uint32_t _actorNum = g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
      std::vector<physx::PxRigidActor*> _actor_V(_actorNum);
      g_pNowScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<physx::PxActor**>(_actor_V.data()), _actorNum);

      for (unsigned int i = 1; i<_actorNum; i++) // ������ �����ϰ� ������
      {
         _actor_V[i]->release();
      }
   }

   // �������Ұ�
   //g_pNowScene->ResetPhysics();
   //Managers::GetInstance()->GetCameraManager()->RemoveAllCamera();
   g_pNowScene = _scene->second;
   //m_NowScene->Initialize();

}

/// <summary>
/// physicsActor�� �޾ƿͼ� phsyx Scene�� �־���
/// shape, rigid type���� �з�
/// </summary>
/// <param name="object">������Ʈ �����ڿ��� Geo, shape���� ������</param>
void PhysicsEngine::AddPhysicsActor(PhysicsActor* object)
{
   physx::PxTransform _pTransform = CreatePxTransform(*object->m_Transform);

   physx::PxGeometry* _nowPxGeo;
   physx::PxShape* _shape;
   switch (object->GetGeometry()->GetType())
   {
      case Sphere:
      {
         _nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
         SphereGeometry* _sphere = dynamic_cast<SphereGeometry*>(object->GetGeometry());
         if (_sphere == nullptr)
         {
            _shape = g_pPhysics->createShape(physx::PxSphereGeometry(), *g_pMaterial);
            break;
         }
         _shape = g_pPhysics->createShape(physx::PxSphereGeometry(_sphere->GetSphereRadius()), *g_pMaterial);
         break;
      }
      case Box:
      {
         _nowPxGeo = CreateGeometry<physx::PxBoxGeometry>();
         BoxGeometry* _box = dynamic_cast<BoxGeometry*>(object->GetGeometry());
         if (_box == nullptr)
         {
            _shape = g_pPhysics->createShape(physx::PxBoxGeometry(), *g_pMaterial);
            break;
         }
         _shape = g_pPhysics->createShape(physx::PxBoxGeometry(_box->GetBoxSize().x, _box->GetBoxSize().y, _box->GetBoxSize().z), *g_pMaterial);
         break;
      }
      case Capsule:
      {
         _nowPxGeo = CreateGeometry<physx::PxCapsuleGeometry>();
         CapsuleGeometry* _capsule = dynamic_cast<CapsuleGeometry*>(object->GetGeometry());
         if (_capsule == nullptr)
         {
            _shape = g_pPhysics->createShape(physx::PxCapsuleGeometry(), *g_pMaterial);
            break;
         }
         _shape = g_pPhysics->createShape(physx::PxCapsuleGeometry(_capsule->GetCapsuleRadius(), _capsule->GetCapsuleHeight()), *g_pMaterial);
         break;
      }

      default:
      {
         _nowPxGeo = CreateGeometry<physx::PxSphereGeometry>();
         SphereGeometry* _sphere = dynamic_cast<SphereGeometry*>(object->GetGeometry());
         if (_sphere == nullptr)
         {
            _shape = g_pPhysics->createShape(physx::PxSphereGeometry(), *g_pMaterial);
            break;
         }
         _shape = g_pPhysics->createShape(physx::PxSphereGeometry(_sphere->GetSphereRadius()), *g_pMaterial);
         break;
      }
   }

   switch (object->GetRigidType())
   {
      case Static: // ���Կ� ������ ���� ����
      {
         physx::PxRigidStatic* _body = g_pPhysics->createRigidStatic(_pTransform);
         _body->userData = (void*)object->GetUserIndex();
         _body->attachShape(*_shape);

         g_pNowScene->addActor(*_body);
         m_PhysicsActors_V.push_back(object);
         break;
      }

      case Dynamic: // ������ �������� ����
      default:
      {
         physx::PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pTransform);
         _body->userData = (void*)object->GetUserIndex();
         _body->attachShape(*_shape);
         physx::PxRigidBodyExt::updateMassAndInertia(*_body, 10.0f);

         g_pNowScene->addActor(*_body);
         m_PhysicsActors_V.push_back(object);
         break;
      }
   }
}

/// <summary>
/// �Ⱦ��� �Լ�
/// </summary>
/// <param name="transform"></param>
void PhysicsEngine::SetPhysicsTransform(Transform* transform)
{
   // ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
   uint32_t _actorNum = g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
   std::vector<physx::PxRigidActor*> _actor_V(_actorNum);
   g_pNowScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
      reinterpret_cast<physx::PxActor**>(_actor_V.data()), _actorNum);

   _actor_V[transform->GetComponent<PhysicsActor>()->GetUserIndex()]->setGlobalPose(CreatePxTransform(*transform));
}

/// <summary>
/// �������� Ʈ�������� ������ Ŭ���� Ʈ�������� ������Ʈ
/// </summary>
/// <param name="actor">���͸� �޾ƿͼ� ������Ʈ</param>
void PhysicsEngine::UpdatePhysicsTransform(PhysicsActor* actor)
{
   // ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
   vector<physx::PxRigidActor*> _actor_V =
      GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

   // PxTransform�� �����ͼ� Renderer Transform�� ������Ʈ 
   UpdateTransformToPhysics(actor->m_Transform, GetPxRigidActor(_actor_V, actor->GetUserIndex())->getGlobalPose()); // -1�� �ϴ°� �����ȵ�
}

/// <summary>
/// Ʈ�������� �ٲ���
/// </summary>
/// <param name="object">������ ������Ʈ</param>
/// <param name="pos">�ٲ� Ʈ������</param>
void PhysicsEngine::SetTransform(GameObject* object, const Transform& trn)
{
   // ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
   std::vector<physx::PxRigidActor*> _actor_V =
      GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

   physx::PxRigidDynamic* _dynamicActor =
      (physx::PxRigidDynamic*)GetPxRigidActor(_actor_V, object->GetComponent<PhysicsActor>()->GetUserIndex());

   _dynamicActor->setGlobalPose(CreatePxTransform(trn));
}

/// <summary>
/// ���� �ӵ� �ִ� �Լ� (AddForce������)
/// </summary>
/// <param name="object">������ ������Ʈ</param>
/// <param name="vel">������ �ӵ�</param>
void PhysicsEngine::SetVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel)
{
   // ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
   vector<physx::PxRigidActor*> _actor_V = GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC);

   physx::PxRigidDynamic* _dynamicActor =
      (physx::PxRigidDynamic*)GetPxRigidActor(_actor_V, object->GetComponent<PhysicsActor>()->GetUserIndex());

   _dynamicActor->setLinearVelocity(CreatePxVec3(vel));
}

/// <summary>
/// ���� ȸ���ӵ� �ִ� �Լ� (AddForce������)
/// </summary>
/// <param name="object">������ ������Ʈ</param>
/// <param name="vel">������ ���ӵ�</param>
void PhysicsEngine::SetAngularVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel)
{
   // ������ ���͸� Ž���ϰ� _actor_V�� �־��ش�
   vector<physx::PxRigidActor*> _actor_V = GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC);

   physx::PxRigidDynamic* _dynamicActor =
      (physx::PxRigidDynamic*)GetPxRigidActor(_actor_V, object->GetComponent<PhysicsActor>()->GetUserIndex());

   _dynamicActor->setAngularVelocity(CreatePxVec3(vel));
}

/// <summary>
/// ���ӿ�����Ʈ ��������
/// </summary>
/// <param name="index">�ε������ؼ�</param>
/// <returns></returns>
GameObject* PhysicsEngine::GetGameObjectByIndex(unsigned int index)
{
   for (PhysicsActor* _actor : m_PhysicsActors_V)
   {
      if (index == _actor->GetUserIndex())
      {
         return _actor->GetMyObject();
      }
   }

   return nullptr;
}

/// <summary>
/// ���������� ��������
/// </summary>
/// <param name="index">�ε��� ���ؼ�</param>
/// <returns></returns>
PhysicsActor* PhysicsEngine::GetPhysicsActorByIndex(unsigned int index)
{
   for (PhysicsActor* _actor : m_PhysicsActors_V)
   {
      if (index == _actor->GetUserIndex())
      {
         return _actor;
      }
   }

   return nullptr;
}

/// <summary>
/// ���ӿ�����Ʈ�κ��� �ε��� ��������
/// </summary>
/// <param name="object"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromGameObject(GameObject* object)
{
   return object->GetComponent<PhysicsActor>()->GetUserIndex();
}

/// <summary>
/// ���������ͷκ��� �ε��� ��������
/// </summary>
/// <param name="actor"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromPhysicsActor(PhysicsActor* actor)
{
   return actor->GetUserIndex();
}

/// collision event�� �ʿ������ ��
//_DLL GameObject* PhysicsEngine::RaycastCheckGameObject(DirectX::SimpleMath::Vector3 rayOrigin,
//   DirectX::SimpleMath::Vector3 rayDir,
//   GeometryBase* geometry, Transform* transform,
//   float maxDist, int hitFlag)
//{
//   physx::PxVec3 _origin = CreatePxVec3(rayOrigin);
//   physx::PxVec3 _dir = CreatePxVec3(rayDir);
//   physx::PxGeometry* _geo = CreatePxGeometry(geometry);
//   physx::PxTransform _trn = CreatePxTransform(*transform);
//   //physx::PxHitFlag::Enum _hit = CreatePxHitFlag((HitFlag)hitFlag);
//   physx::PxHitFlags _hitFlag = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
//   physx::PxRaycastHit _rayHit;
//
//
//   /// �۵��� ���Ѵ�... ��...?
//   physx::PxGeometryQuery::raycast(_origin, _dir, *_geo, _trn, maxDist, _hitFlag, 10, &_rayHit);
//
//
//   //RaycastHit _hitInfo;
//
//   if (_rayHit.actor == nullptr)
//   {
//      return nullptr;
//   }
//   else
//   {
//      return GetPhysicsActorByIndex((int)_rayHit.actor->userData);
//   }
//}

/// <summary>
/// ����ĳ��Ʈ�� ���� ���ӿ�����Ʈ�� ������
/// </summary>
/// <param name="origin">��������</param>
/// <param name="dir">���� ����</param>
/// <param name="dist">���� ����</param>
/// <returns>���̿� ���� ���ӿ�����Ʈ</returns>
GameObject* PhysicsEngine::RaycastCheckGameObject(const DirectX::SimpleMath::Vector3& origin,
   const DirectX::SimpleMath::Vector3& dir, float dist)
{
   physx::PxVec3 _origin = CreatePxVec3(origin);
   physx::PxVec3 _dir = CreatePxVec3(dir);
   physx::PxRaycastBuffer _hitBuffer;
   physx::PxRaycastHit _rayHit;
   physx::PxHitFlags _hitFlag = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;

   g_pNowScene->raycast(_origin, _dir, dist, _hitBuffer, _hitFlag);

   _rayHit = _hitBuffer.block;

   if (_rayHit.actor == nullptr)
   {
      return nullptr;
   }
   else
   {
      return GetGameObjectByIndex((int)_rayHit.actor->userData);
   }
}

/// <summary>
/// ����ĳ��Ʈ��Ʈ�� ���� ���� ��������
/// </summary>
/// <param name="origin">��������</param>
/// <param name="dir">���� ����</param>
/// <param name="dist">���� ����</param>
/// <returns>����ĳ��Ʈ��Ʈ����</returns>
RaycastHit PhysicsEngine::CheckRaycastHit(const DirectX::SimpleMath::Vector3& origin,
   const DirectX::SimpleMath::Vector3& dir, float dist)
{
   physx::PxVec3 _origin = CreatePxVec3(origin);
   physx::PxVec3 _dir = CreatePxVec3(dir);
   physx::PxRaycastBuffer _hitBuffer;
   physx::PxRaycastHit _rayHit;
   physx::PxHitFlags _hitFlag = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;

   g_pNowScene->raycast(_origin, _dir, dist, _hitBuffer, _hitFlag);

   _rayHit = _hitBuffer.block;

   if (_rayHit.actor == nullptr)
   {
      return RaycastHit(); // �󲮵���
   }
   else
   {
      RaycastHit _hitData;
      _hitData.actor = GetPhysicsActorByIndex((int)_rayHit.actor->userData);
      _hitData.faceIndex = _rayHit.faceIndex;
      _hitData.flag = CreateHitFlag(physx::PxHitFlag::ePOSITION); // �ϴ� ����������
      _hitData.position = CreateVector3(_rayHit.position);
      _hitData.normal = CreateVector3(_rayHit.normal);
      _hitData.distance = _rayHit.distance;
      _hitData.u = _rayHit.u;
      _hitData.v = _rayHit.v;

      return _hitData;
   }
}

/// <summary>
/// �� ���Ϳ� �ε��� �͵��� ������ ������(CollisionHit�� ����)
/// ���⼭ start, on, end�� enum�� ������
/// </summary>
/// <param name="data">������ �����̳�</param>
/// <param name="actor">�� ����</param>
/// <param name="transform">�� ���� ��ġ</param>
void PhysicsEngine::CheckCollision(std::vector<CollisionHit>* data, PhysicsActor* actor, Transform* transform)
{
   //physx::PxGeometry* _geo = CreatePxGeometry(actor->GetGeometry());
   physx::PxSphereGeometry _geo(5.1f);/// �б�Ÿ�� ������ �����Ұ� ���� ����
   physx::PxTransform _trans = CreatePxTransform(*transform);

   physx::PxOverlapHit _colHit[10]; // �ϴ� �ӽ÷� 10
   physx::PxMemZero(&_colHit, sizeof(_colHit));
   physx::PxOverlapBuffer _colBuffer(_colHit, 10);

   // �浹�� �͵� ã�Ƽ� _colBuffer�� �־��� (current �浹�� �޾ƿ�)
   g_pNowScene->overlap(_geo, _trans, _colBuffer);
   unsigned int _hitNum = _colBuffer.getNbAnyHits();

   vector<CollisionHit> _currColHit(_hitNum);


   // �ݸ����� �Ͼ�� ������Ʈ loop (physx���� �޾ƿ� �浹 ������) current�����Ϳ� ����
   // �ݸ����� ó�� �Ͼ�� actor�� ����
   for (unsigned int i = 0; i < _hitNum; i++)
   {
      _colHit[i] = _colBuffer.getAnyHit(i);
      int _idx = (int)_colHit[i].actor->userData;

      // �ٴڰ� �ڱ��ڽ��� ��ŵ
      if (_idx == 0 || _idx == actor->GetUserIndex())
      {
         continue;
      }

      if (data->size() > 0) // �浹 �߿� �ٸ� �浹 ó��
      {
         //for (CollisionHit _prevHit : *data)
         for (int j = 0; j < data->size(); j++)
         {
            // ���� �̹� �ִ� actor��
            if (_idx == data->at(j).actor->GetUserIndex())
            {
               data->at(j).currColState = CollisionState::OnCollision;
               break;
            }

            // ���� ���͸� �־��ش�
            CollisionHit _newHitData;
            _newHitData.actor = GetPhysicsActorByIndex(_idx);
            _newHitData.currColState = CollisionState::StartCollision;
            data->push_back(_newHitData);
         }
      }
      else // ���� ó�� �浹�̸�
      {
         // ���� ���͸� �־��ش�
         CollisionHit _newHitData;
         _newHitData.actor = GetPhysicsActorByIndex(_idx);
         _newHitData.currColState = CollisionState::StartCollision;
         data->push_back(_newHitData);
      }
   }

   bool _isIn = false; // �̹� ���°� ��ȯ�Ȱ��� Ȯ���ϴ� �Ұ�
   for (int i = 0; i < data->size(); i++)
   {
      // end collision�̸� �׸��� 
      if (data->at(i).currColState == CollisionState::EndCollision)
      {
         data->erase(data->begin() + i);
         continue;
      }

      for (unsigned int j = 0; j < _hitNum; j++)
      {
         _colHit[j] = _colBuffer.getAnyHit(j);
         int _idx = (int)_colHit[j].actor->userData;

         // �ٴڰ� �ڱ��ڽ��� ��ŵ
         if (_idx == 0 || _idx == actor->GetUserIndex())
         {
            continue;
         }

         if (_idx == data->at(i).actor->GetUserIndex()) // ???
         {
            _isIn = true;
            break;
         }
      }

      if (_isIn == false)
      {
         data->at(i).currColState = CollisionState::EndCollision;
         //data->erase(data->begin() + i);
      }
   }
}


//void PhysicsEngine::CheckCollision(std::vector<CollisionHit>* data, PhysicsActor* actor, Transform* transform)
//{
//   //physx::PxGeometry* _geo = CreatePxGeometry(actor->GetGeometry());
//   physx::PxSphereGeometry _geo(5.1f);
//   physx::PxTransform _trans = CreatePxTransform(*transform);
//
//   physx::PxOverlapHit _colHit[10]; // �ϴ� �ӽ÷� 10
//   physx::PxMemZero(&_colHit, sizeof(_colHit));
//   physx::PxOverlapBuffer _colBuffer(_colHit, 10);
//
//   // �浹�� �͵� ã�Ƽ� _colBuffer�� �־��� (currendt �浹�� �޾ƿ�)
//   g_pNowScene->overlap(_geo, _trans, _colBuffer);
//   unsigned int _hitNum = _colBuffer.getNbAnyHits();
//
//   // �ݸ����� �Ͼ�� ������Ʈ loop (physx���� �޾ƿ� �浹 ������)
//   for (unsigned int i = 0; i < _hitNum; i++)
//   {
//      _colHit[i] = _colBuffer.getAnyHit(i);
//      int _idx = (int)_colHit[i].actor->userData;
//
//      // �ٴڰ� �ڱ��ڽ��� ��ŵ
//      if (_idx == 0 || _idx == actor->GetUserIndex())
//      {
//         continue;
//      }
//
//      CollisionHit _newHitData;
//      _newHitData.actor = GetPhysicsActorByIndex(_idx);
//
//      data->push_back(_newHitData);
//   }
//}
