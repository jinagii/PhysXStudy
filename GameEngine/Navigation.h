#pragma once

#include "DXObjectBase.h"
#include "DXDefine.h"

/// 네비게이션 메쉬
// 단일 폐곡선 모양 삼각형으로된 그룹 (wire frame)
// 삼각형을 모두 순환한다
// 네비메쉬에 수직 y축에 평행한 직선과 충돌감지를 한다
// return값으로 vector3

/// 삼각형 안에 점이 있는지 확인하는 방법(dot, cross이용)
// 점 a, b, c로 이뤄진 삼각형과 점 p
// (ab x ap) dot (ap x ac) 의 값이 모든 꼭지점에서 양수면 p는 삼각형 안에있다

/// 뭐야..이게 뭐냐고 ㅠㅠ

class Camera;
class ASEParser;

struct Triangle
{
   // triangle vertex positions
   Vector3 a;
   Vector3 b;
   Vector3 c;

   UINT aIdx = 0;
   UINT bIdx = 0;
   UINT cIdx = 0;

   int index = 0;

   ID3D11Buffer* pVBTriangle = nullptr;
   ID3D11Buffer* pIBTriangle = nullptr;
};

class Navigation : public DXObjectBase
{
public:
   Navigation();
   virtual ~Navigation();

   virtual void Initialize(ASEParser* pASEParserData);
   virtual void Update(Camera* pCamera, float dTime);
   virtual void Render();

   void CreateVBIBOfTriangle(Triangle* triangle);
      
   // MK1
   Vector3 PlayerPositionUpdate(Vector3 pos);
   int CheckTriangleNumberOnNaviMesh(Vector3 pos);
   Triangle* CheckTriangleOnNaviMesh(Vector3 pos);

   // MK2
   Triangle* CheckTriangleOnNaviMesh2(Vector3 pos);
   Vector3 VectorToKeepInTriangle(Vector3 pos);

   // MK3
   Triangle* CheckTriangleOnNaviMesh3(Vector3 pos);
   Vector3 PositionInTriangle(Vector3 pos, Triangle* triangle);
   Vector3 NormalVectorOfTriangle(Triangle* triangle);

   bool GetIsInTriangle() const { return m_IsInTriangle; }
   void SetIsInTriangle(bool val) { m_IsInTriangle = val; }

private:
   // 처음에 필요할거라 생각했던것들
   //Vector3 m_NaviRay; // Y축에 평행한 Ray
   //Vector3 m_NowPosition; // 현재 위치값
   //Vector3 m_LastPosition; // 맵을 벗어나려하면 되돌릴 좌표? 
   
   // 현재 쓰는것들
   Mesh* m_NaviMeshData;

   vector<Triangle*> m_Triangles_v;
   Triangle* m_pNowTriangle;   
   bool m_IsInTriangle;

   Vector3 m_PlayerPosition;

   UINT m_IndexCount;
};

