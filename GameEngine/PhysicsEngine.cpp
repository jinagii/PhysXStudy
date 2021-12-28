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
/// 이거는 Managers에서 해줌
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
   //_sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // 중력값
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

   // 머티리얼 설정 staticFriction, dynamicFriction, restitution
   g_pMaterial = g_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

}

/// <summary>
/// 이것도 Managers -> Engine에서 해줌
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::PhysicsLoop(bool interactive)
{
   g_pNowScene->simulate(1.0f / 60.0f);
   g_pNowScene->fetchResults(true); // 물리를 쓸건가 안쓸건가?

   for (PhysicsActor* _actor : m_PhysicsActors_V)
   {
      UpdatePhysicsTransform(_actor);
   }


}

/// <summary>
/// 끝낼때 쓰는 릴리즈
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
/// 주로 씬전환할때 액터들 정리
/// </summary>
/// <param name="interactive"></param>
void PhysicsEngine::ReleasePhysicsActor(bool interactive)
{
   // 일단 전체클리어


   m_PhysicsActors_V.clear();



   /// for문으로 해제 필요




}

/// <summary>
/// 실험용으로 만들었던것 (참고용), pvd에서는 작동됨
/// 게임 런타임 중에 physicsActor가 동적으로 만들어지는지 확인하기위해 만들었음
/// </summary>
/// <param name="transform"></param>
/// <param name="size"></param>
void PhysicsEngine::CreateTestBox(Transform* transform, float size)
{
   physx::PxTransform _pxTr = CreatePxTransform(*transform);

   physx::PxShape* _shape = g_pPhysics->createShape(physx::PxBoxGeometry(size, size, size), *g_pMaterial);
   //physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(0, 1, 0, 0), *g_pMaterial); // 지면
   physx::PxRigidStatic* _body = physx::PxCreateStatic(*g_pPhysics, _pxTr, *_shape);
   //physx::PxRigidDynamic* _body = g_pPhysics->createRigidDynamic(_pxTr);

   _body->attachShape(*_shape);

   g_pNowScene->addActor(*_body);
}
/// <summary>
/// 참고용 (실제 안씀)
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

   /// 아래 코드는 안만들어진다 why?
   //physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*g_pPhysics, _pxTr, *_nowPxGeo, *g_pMaterial, 10.0f); 

   /// 아래 코드는 뭔가 만들어짐
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
/// 참고용 (실제 안씀)
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
/// Managers에서 해줌 (씬이 isPhysicsScene이면 알아서 등록)
/// Tolerance하나정하면 그대로 씀
/// </summary>
/// <param name="scene"></param>
void PhysicsEngine::AddPhysicsScene(SceneBase* scene)
{
   static bool _isFirstScene = true;

   physx::PxSceneDesc _sceneDesc(g_pPhysics->getTolerancesScale());
   _sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // 중력값
   /// 이용가능한 CPU의 쓰레드 개수를 가져오는 윈도우함수를 써보자
   g_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
   _sceneDesc.cpuDispatcher = g_pDispatcher;
   _sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; /// 뭔지모르겠다...
   physx::PxScene* _pScene = g_pPhysics->createScene(_sceneDesc);

   // rigid 지면을 만들어 준다
   physx::PxRigidStatic* _groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(0, 1, 0, 0), *g_pMaterial);
   /// 지면만드는것 확인해보기
   //physx::PxVec3 _p1(0.0f, 0.0f, 10.0f);
   //physx::PxVec3 _p2(-10.0f, 0.0f, -10.0f);
   //physx::PxVec3 _p3(10.0f, 0.0f, -10.0f);
   //physx::PxRigidStatic* _groundPlane = physx::PxCreatePlane(*g_pPhysics, physx::PxPlane(_p1, _p2, _p3), *g_pMaterial);
   _pScene->addActor(*_groundPlane); // 지면을 물리 엔진에 추가

   g_pScene_UM.insert({ scene->GetSceneName(), _pScene });

   if (_isFirstScene == true)
   {
      SetNowPhysicsScene(scene->GetSceneName());
      _isFirstScene = false;
   }
}

/// <summary>
/// 알아서 되게끔 수정필요
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

      for (unsigned int i = 1; i<_actorNum; i++) // 지면을 제외하고 릴리즈
      {
         _actor_V[i]->release();
      }
   }

   // 만들어야할것
   //g_pNowScene->ResetPhysics();
   //Managers::GetInstance()->GetCameraManager()->RemoveAllCamera();
   g_pNowScene = _scene->second;
   //m_NowScene->Initialize();

}

/// <summary>
/// physicsActor를 받아와서 phsyx Scene에 넣어줌
/// shape, rigid type등을 분류
/// </summary>
/// <param name="object">오브젝트 생성자에서 Geo, shape들을 가져옴</param>
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
      case Static: // 무게와 관성의 영향 없음
      {
         physx::PxRigidStatic* _body = g_pPhysics->createRigidStatic(_pTransform);
         _body->userData = (void*)object->GetUserIndex();
         _body->attachShape(*_shape);

         g_pNowScene->addActor(*_body);
         m_PhysicsActors_V.push_back(object);
         break;
      }

      case Dynamic: // 가변적 물리량을 가짐
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
/// 안쓰는 함수
/// </summary>
/// <param name="transform"></param>
void PhysicsEngine::SetPhysicsTransform(Transform* transform)
{
   // 씬에서 액터를 탐색하고 _actor_V에 넣어준다
   uint32_t _actorNum = g_pNowScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
   std::vector<physx::PxRigidActor*> _actor_V(_actorNum);
   g_pNowScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
      reinterpret_cast<physx::PxActor**>(_actor_V.data()), _actorNum);

   _actor_V[transform->GetComponent<PhysicsActor>()->GetUserIndex()]->setGlobalPose(CreatePxTransform(*transform));
}

/// <summary>
/// 피직스쪽 트랜스폼을 가져와 클라쪽 트랜스폼을 업데이트
/// </summary>
/// <param name="actor">액터를 받아와서 업데이트</param>
void PhysicsEngine::UpdatePhysicsTransform(PhysicsActor* actor)
{
   // 씬에서 액터를 탐색하고 _actor_V에 넣어준다
   vector<physx::PxRigidActor*> _actor_V =
      GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

   // PxTransform을 가져와서 Renderer Transform을 업데이트 
   UpdateTransformToPhysics(actor->m_Transform, GetPxRigidActor(_actor_V, actor->GetUserIndex())->getGlobalPose()); // -1을 하는게 맘에안듬
}

/// <summary>
/// 트랜스폼을 바꿔줌
/// </summary>
/// <param name="object">변경할 오브젝트</param>
/// <param name="pos">바꿀 트랜스폼</param>
void PhysicsEngine::SetTransform(GameObject* object, const Transform& trn)
{
   // 씬에서 액터를 탐색하고 _actor_V에 넣어준다
   std::vector<physx::PxRigidActor*> _actor_V =
      GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

   physx::PxRigidDynamic* _dynamicActor =
      (physx::PxRigidDynamic*)GetPxRigidActor(_actor_V, object->GetComponent<PhysicsActor>()->GetUserIndex());

   _dynamicActor->setGlobalPose(CreatePxTransform(trn));
}

/// <summary>
/// 일정 속도 주는 함수 (AddForce같은것)
/// </summary>
/// <param name="object">변경할 오브젝트</param>
/// <param name="vel">변경할 속도</param>
void PhysicsEngine::SetVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel)
{
   // 씬에서 액터를 탐색하고 _actor_V에 넣어준다
   vector<physx::PxRigidActor*> _actor_V = GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC);

   physx::PxRigidDynamic* _dynamicActor =
      (physx::PxRigidDynamic*)GetPxRigidActor(_actor_V, object->GetComponent<PhysicsActor>()->GetUserIndex());

   _dynamicActor->setLinearVelocity(CreatePxVec3(vel));
}

/// <summary>
/// 일정 회전속도 주는 함수 (AddForce같은것)
/// </summary>
/// <param name="object">변경할 오브젝트</param>
/// <param name="vel">변경할 각속도</param>
void PhysicsEngine::SetAngularVelocity(GameObject* object, const DirectX::SimpleMath::Vector3& vel)
{
   // 씬에서 액터를 탐색하고 _actor_V에 넣어준다
   vector<physx::PxRigidActor*> _actor_V = GetPxRigidActorVector(g_pNowScene, physx::PxActorTypeFlag::eRIGID_DYNAMIC);

   physx::PxRigidDynamic* _dynamicActor =
      (physx::PxRigidDynamic*)GetPxRigidActor(_actor_V, object->GetComponent<PhysicsActor>()->GetUserIndex());

   _dynamicActor->setAngularVelocity(CreatePxVec3(vel));
}

/// <summary>
/// 게임오브젝트 가져오기
/// </summary>
/// <param name="index">인덱스통해서</param>
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
/// 피직스액터 가져오기
/// </summary>
/// <param name="index">인덱스 통해서</param>
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
/// 게임오브젝트로부터 인덱스 가져오기
/// </summary>
/// <param name="object"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromGameObject(GameObject* object)
{
   return object->GetComponent<PhysicsActor>()->GetUserIndex();
}

/// <summary>
/// 피직스액터로부터 인덱스 가져오기
/// </summary>
/// <param name="actor"></param>
/// <returns></returns>
unsigned int PhysicsEngine::GetIndexFromPhysicsActor(PhysicsActor* actor)
{
   return actor->GetUserIndex();
}

/// collision event가 필요없을때 씀
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
//   /// 작동을 안한다... 왜...?
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
/// 레이캐스트를 쏴서 게임오브젝트를 가져옴
/// </summary>
/// <param name="origin">내포지션</param>
/// <param name="dir">보는 방향</param>
/// <param name="dist">방향 길이</param>
/// <returns>레이에 닿은 게임오브젝트</returns>
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
/// 레이캐스트히트에 대한 정보 가져오기
/// </summary>
/// <param name="origin">내포지션</param>
/// <param name="dir">보는 방향</param>
/// <param name="dist">방향 길이</param>
/// <returns>레이캐스트히트정보</returns>
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
      return RaycastHit(); // 빈껍데기
   }
   else
   {
      RaycastHit _hitData;
      _hitData.actor = GetPhysicsActorByIndex((int)_rayHit.actor->userData);
      _hitData.faceIndex = _rayHit.faceIndex;
      _hitData.flag = CreateHitFlag(physx::PxHitFlag::ePOSITION); // 일단 포지션으로
      _hitData.position = CreateVector3(_rayHit.position);
      _hitData.normal = CreateVector3(_rayHit.normal);
      _hitData.distance = _rayHit.distance;
      _hitData.u = _rayHit.u;
      _hitData.v = _rayHit.v;

      return _hitData;
   }
}

/// <summary>
/// 내 액터와 부딧힌 것들의 정보를 가져옴(CollisionHit에 저장)
/// 여기서 start, on, end의 enum을 정해줌
/// </summary>
/// <param name="data">저장할 컨테이너</param>
/// <param name="actor">내 액터</param>
/// <param name="transform">내 액터 위치</param>
void PhysicsEngine::CheckCollision(std::vector<CollisionHit>* data, PhysicsActor* actor, Transform* transform)
{
   //physx::PxGeometry* _geo = CreatePxGeometry(actor->GetGeometry());
   physx::PxSphereGeometry _geo(5.1f);/// 분기타서 사이즈 조절할것 오늘 ㄱㄱ
   physx::PxTransform _trans = CreatePxTransform(*transform);

   physx::PxOverlapHit _colHit[10]; // 일단 임시로 10
   physx::PxMemZero(&_colHit, sizeof(_colHit));
   physx::PxOverlapBuffer _colBuffer(_colHit, 10);

   // 충돌된 것들 찾아서 _colBuffer에 넣어줌 (current 충돌을 받아옴)
   g_pNowScene->overlap(_geo, _trans, _colBuffer);
   unsigned int _hitNum = _colBuffer.getNbAnyHits();

   vector<CollisionHit> _currColHit(_hitNum);


   // 콜리션이 일어나는 오브젝트 loop (physx에서 받아온 충돌 데이터) current데이터와 같음
   // 콜리션이 처음 일어나는 actor를 관리
   for (unsigned int i = 0; i < _hitNum; i++)
   {
      _colHit[i] = _colBuffer.getAnyHit(i);
      int _idx = (int)_colHit[i].actor->userData;

      // 바닥과 자기자신은 스킵
      if (_idx == 0 || _idx == actor->GetUserIndex())
      {
         continue;
      }

      if (data->size() > 0) // 충돌 중에 다른 충돌 처리
      {
         //for (CollisionHit _prevHit : *data)
         for (int j = 0; j < data->size(); j++)
         {
            // 만약 이미 있는 actor면
            if (_idx == data->at(j).actor->GetUserIndex())
            {
               data->at(j).currColState = CollisionState::OnCollision;
               break;
            }

            // 없는 액터면 넣어준다
            CollisionHit _newHitData;
            _newHitData.actor = GetPhysicsActorByIndex(_idx);
            _newHitData.currColState = CollisionState::StartCollision;
            data->push_back(_newHitData);
         }
      }
      else // 완전 처음 충돌이면
      {
         // 없는 액터면 넣어준다
         CollisionHit _newHitData;
         _newHitData.actor = GetPhysicsActorByIndex(_idx);
         _newHitData.currColState = CollisionState::StartCollision;
         data->push_back(_newHitData);
      }
   }

   bool _isIn = false; // 이미 상태가 변환된건지 확인하는 불값
   for (int i = 0; i < data->size(); i++)
   {
      // end collision이면 항목을 
      if (data->at(i).currColState == CollisionState::EndCollision)
      {
         data->erase(data->begin() + i);
         continue;
      }

      for (unsigned int j = 0; j < _hitNum; j++)
      {
         _colHit[j] = _colBuffer.getAnyHit(j);
         int _idx = (int)_colHit[j].actor->userData;

         // 바닥과 자기자신은 스킵
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
//   physx::PxOverlapHit _colHit[10]; // 일단 임시로 10
//   physx::PxMemZero(&_colHit, sizeof(_colHit));
//   physx::PxOverlapBuffer _colBuffer(_colHit, 10);
//
//   // 충돌된 것들 찾아서 _colBuffer에 넣어줌 (currendt 충돌을 받아옴)
//   g_pNowScene->overlap(_geo, _trans, _colBuffer);
//   unsigned int _hitNum = _colBuffer.getNbAnyHits();
//
//   // 콜리션이 일어나는 오브젝트 loop (physx에서 받아온 충돌 데이터)
//   for (unsigned int i = 0; i < _hitNum; i++)
//   {
//      _colHit[i] = _colBuffer.getAnyHit(i);
//      int _idx = (int)_colHit[i].actor->userData;
//
//      // 바닥과 자기자신은 스킵
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
