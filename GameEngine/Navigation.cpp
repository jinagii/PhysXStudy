#include "Camera.h"
#include "Effect.h"
#include "ParsingData.h"
#include "ASEParser.h"
#include "StructAndConst.h"
#include "DXRenderState.h"
#include "DXDevice.h"
#include "Navigation.h"

Navigation::Navigation()
   : DXObjectBase(),
   m_IsInTriangle(false), m_IndexCount(), m_NaviMeshData(),
   m_pNowTriangle()
{
}

Navigation::~Navigation()
{
}

void Navigation::Initialize(ASEParser* pASEParserData)
{
   for (Mesh* _meshDataIter : pASEParserData->m_MeshData_v) // 네비메쉬 불러오고 넣어준다
   {
      m_NaviMeshData = _meshDataIter;
   }

   if (m_NaviMeshData->m_OptimizedVertices_v.size() <= 0)
   {
      return;
   }

   UINT vcount = 0;
   UINT tcount = 0;

   // 네비의 버텍스 정보 넣어준다
   vcount = m_NaviMeshData->m_OptimizedVertices_v.size();

   vector<PosColor> vertices(vcount);
   for (UINT i = 0; i < vcount; i++)
   {      
      vertices[i].Pos.x = m_NaviMeshData->m_OptimizedVertices_v[i]->m_Pos.x;
      //m_NaviMeshData->m_OptimizedVertices_v[i]->m_Pos.y = m_NaviMeshData->m_OptimizedVertices_v[i]->m_Pos.y + 0.01f;
      vertices[i].Pos.y = m_NaviMeshData->m_OptimizedVertices_v[i]->m_Pos.y;
      vertices[i].Pos.z = m_NaviMeshData->m_OptimizedVertices_v[i]->m_Pos.z;

      vertices[i].Color = Vector4((const float*)&Colors::Blue);
   }

   D3D11_BUFFER_DESC vbd;
   vbd.Usage = D3D11_USAGE_IMMUTABLE;
   vbd.ByteWidth = sizeof(PosColor) * vcount;
   vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   vbd.CPUAccessFlags = 0;
   vbd.MiscFlags = 0;
   vbd.StructureByteStride = 0;
   D3D11_SUBRESOURCE_DATA vinitData;
   vinitData.pSysMem = &vertices[0];
   HR(DXDevice::g_Device->CreateBuffer(&vbd, &vinitData, &m_pVB));

   //////////////////// index

   tcount = m_NaviMeshData->m_MeshNumFace;
   m_IndexCount = 3 * tcount;
   vector<UINT> indices(m_IndexCount);
   for (UINT i = 0; i < tcount; ++i)
   {
      indices[i * 3 + 0] = m_NaviMeshData->m_OptimizedIndices[i].Index[0];
      indices[i * 3 + 1] = m_NaviMeshData->m_OptimizedIndices[i].Index[2];
      indices[i * 3 + 2] = m_NaviMeshData->m_OptimizedIndices[i].Index[1];

      // 삼각형을 만들고 넣자
      Triangle* _triangle = new Triangle();
      _triangle->aIdx = m_NaviMeshData->m_OptimizedIndices[i].Index[0];
      _triangle->bIdx = m_NaviMeshData->m_OptimizedIndices[i].Index[2];
      _triangle->cIdx = m_NaviMeshData->m_OptimizedIndices[i].Index[1];
      _triangle->index = i;
      m_Triangles_v.push_back(_triangle);
   }

   // Pack the indices of all the meshes into one index buffer.
   D3D11_BUFFER_DESC ibd;
   ibd.Usage = D3D11_USAGE_IMMUTABLE;
   ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
   ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
   ibd.CPUAccessFlags = 0;
   ibd.MiscFlags = 0;
   ibd.StructureByteStride = 0;
   D3D11_SUBRESOURCE_DATA iinitData;
   iinitData.pSysMem = &indices[0];
   HR(DXDevice::g_Device->CreateBuffer(&ibd, &iinitData, &m_pIB));
}

void Navigation::Update(Camera* pCamera, float dTime)
{
   m_World = XMMatrixIdentity();
   m_View = pCamera->View();
   m_Proj = pCamera->Proj();

   m_pNowTriangle = CheckTriangleOnNaviMesh3(m_PlayerPosition);
}

void Navigation::Render()
{
   /// NaviMesh ////////////////////////////////////////////////////////
   DXDevice::g_DeviceContext->IASetInputLayout(InputLayout::PosColor);
   DXDevice::g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

   // 인덱스버퍼와 버텍스버퍼 셋팅
   UINT stride = sizeof(PosColor);
   UINT offset = 0;
   DXDevice::g_DeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
   DXDevice::g_DeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
   // 렌더스테이트
   DXDevice::g_DeviceContext->RSSetState(DXRenderState::g_WireFrame);

   // WVP TM등을 셋팅
   Matrix worldViewProj = m_World * m_View * m_Proj;
   StaticEffect::ColorFX->SetWorldViewProj(worldViewProj);
   m_pTech = StaticEffect::ColorFX->LightTech;

   D3DX11_TECHNIQUE_DESC techDesc;
   m_pTech->GetDesc(&techDesc);
   for (UINT p = 0; p < techDesc.Passes; ++p)
   {
      m_pTech->GetPassByIndex(p)->Apply(0, DXDevice::g_DeviceContext);
      // 인덱스로 그린다
      DXDevice::g_DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
   }

   /*
   ID3D11Buffer* VB;
   ID3D11Buffer* IB;

   int index = CheckTriangleNumberOnNaviMesh(m_PlayerPosition);
   if (index != -1)
   {
      VB = m_Triangles_v[index]->pVBTriangle;
      IB = m_Triangles_v[index]->pIBTriangle;

      m_pDeviceContext->IASetVertexBuffers(0, 1, &VB, &_stride, &_offset);
      m_pDeviceContext->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, 0);

      m_pDeviceContext->RSSetState(DXRenderState::g_WireFrame);

      D3DX11_TECHNIQUE_DESC _techDesc;
      m_pTech->GetDesc(&_techDesc);

      for (UINT p = 0; p < _techDesc.Passes; ++p)
      {
         m_pTech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

         m_pDeviceContext->DrawIndexed(3, 0, 0);
      }
   }
   */

   /// triangle ////////////////////////////////////////////////////////////////
   if (m_pNowTriangle != nullptr)
   {
      CreateVBIBOfTriangle(m_pNowTriangle);
      //m_pDeviceContext->IASetInputLayout(InputLayout::PosColor);
      //m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      UINT _stride = sizeof(PosColor);
      UINT _offset = 0;
      DXDevice::g_DeviceContext->IASetVertexBuffers(0, 1, &m_pNowTriangle->pVBTriangle, &_stride, &_offset);
      DXDevice::g_DeviceContext->IASetIndexBuffer(m_pNowTriangle->pIBTriangle, DXGI_FORMAT_R32_UINT, 0);
      // 렌더스테이트
      DXDevice::g_DeviceContext->RSSetState(DXRenderState::g_SolidFrame);

      // WVP TM등을 셋팅
      Matrix _worldViewProj = m_World * m_View * m_Proj;
      StaticEffect::ColorFX->SetWorldViewProj(_worldViewProj);
      m_pTech = StaticEffect::ColorFX->LightTech;  

      D3DX11_TECHNIQUE_DESC _techDesc;
      m_pTech->GetDesc(&_techDesc);
      for (UINT p = 0; p < _techDesc.Passes; ++p)
      {
         m_pTech->GetPassByIndex(p)->Apply(0, DXDevice::g_DeviceContext);

         DXDevice::g_DeviceContext->DrawIndexed(3, 0, 0);
      }
   }
}

Vector3 Navigation::PlayerPositionUpdate(Vector3 pos)
{
   return m_PlayerPosition = pos;
}

int Navigation::CheckTriangleNumberOnNaviMesh(Vector3 pos)
{
   float dotA = 0.0f; // a에서의 dot
   float dotB = 0.0f; // b에서의 dot
   float dotC = 0.0f; // c에서의 dot

   //for (Triangle _iter : m_Triangles_v)
   for (int i = 0; i < m_Triangles_v.size(); i++)
   {
      Vector3 crossAB = (m_Triangles_v[i]->b - m_Triangles_v[i]->a).Cross(pos - m_Triangles_v[i]->a);
      Vector3 crossAC = (pos - m_Triangles_v[i]->a).Cross(m_Triangles_v[i]->c - m_Triangles_v[i]->a);
      dotA = crossAB.Dot(crossAC);

      Vector3 crossBA = (m_Triangles_v[i]->a - m_Triangles_v[i]->b).Cross(pos - m_Triangles_v[i]->b);
      Vector3 crossBC = (pos - m_Triangles_v[i]->b).Cross(m_Triangles_v[i]->c - m_Triangles_v[i]->b);
      dotB = crossBA.Dot(crossBC);

      Vector3 crossCB = (m_Triangles_v[i]->b - m_Triangles_v[i]->c).Cross(pos - m_Triangles_v[i]->c);
      Vector3 crossCA = (pos - m_Triangles_v[i]->c).Cross(m_Triangles_v[i]->a - m_Triangles_v[i]->c);
      dotC = crossCB.Dot(crossCA);

      if (dotA > 0.0f && dotB > 0.0f && dotC > 0.0f) // 모두 양수면 삼각형 안에 있는것
      {
         return i; // i번째 삼각형 안에 개미가 존재
      }
   }

   return (-1); // 실패 음수로 처리
}

Triangle* Navigation::CheckTriangleOnNaviMesh(Vector3 pos)
{
   float dotA = 0.0f; // a에서의 dot
   float dotB = 0.0f; // b에서의 dot
   float dotC = 0.0f; // c에서의 dot

   //for (Triangle _iter : m_Triangles_v)
   for (int i = 0; i < m_Triangles_v.size(); i++)
   {
      Vector3 crossAB = (m_Triangles_v[i]->b - m_Triangles_v[i]->a).Cross(pos - m_Triangles_v[i]->a);
      Vector3 crossAC = (pos - m_Triangles_v[i]->a).Cross(m_Triangles_v[i]->c - m_Triangles_v[i]->a);
      dotA = crossAB.Dot(crossAC);

      Vector3 crossBA = (m_Triangles_v[i]->a - m_Triangles_v[i]->b).Cross(pos - m_Triangles_v[i]->b);
      Vector3 crossBC = (pos - m_Triangles_v[i]->b).Cross(m_Triangles_v[i]->c - m_Triangles_v[i]->b);
      dotB = crossBA.Dot(crossBC);

      Vector3 crossCB = (m_Triangles_v[i]->b - m_Triangles_v[i]->c).Cross(pos - m_Triangles_v[i]->c);
      Vector3 crossCA = (pos - m_Triangles_v[i]->c).Cross(m_Triangles_v[i]->a - m_Triangles_v[i]->c);
      dotC = crossCB.Dot(crossCA);

      if (dotA > 0.0f && dotB > 0.0f && dotC > 0.0f) // 모두 양수면 삼각형 안에 있는것
      {
         return m_Triangles_v[i]; // i번째 삼각형 안에 개미가 존재
      }
   }

   return nullptr; // 실패 음수로 처리
}

Triangle* Navigation::CheckTriangleOnNaviMesh2(Vector3 pos)
{
   float dotA = 0.0f; // a에서의 dot
   float dotB = 0.0f; // b에서의 dot
   float dotC = 0.0f; // c에서의 dot

   for (int i = 0; i < m_Triangles_v.size(); i++)
   {
      // 삼각형의 각 꼭지점
      Vector3 nowTrngA = m_NaviMeshData->m_OptimizedVertices_v[m_Triangles_v[i]->aIdx]->m_Pos;
      Vector3 nowTrngB = m_NaviMeshData->m_OptimizedVertices_v[m_Triangles_v[i]->bIdx]->m_Pos;
      Vector3 nowTrngC = m_NaviMeshData->m_OptimizedVertices_v[m_Triangles_v[i]->cIdx]->m_Pos;

      Vector3 crossAB = (nowTrngB - nowTrngA).Cross(pos - nowTrngA);
      Vector3 crossAC = (pos - nowTrngA).Cross(nowTrngC - nowTrngA);
      dotA = crossAB.Dot(crossAC);

      Vector3 crossBA = (nowTrngA - nowTrngB).Cross(pos - nowTrngB);
      Vector3 crossBC = (pos - nowTrngB).Cross(nowTrngC - nowTrngB);
      dotB = crossBA.Dot(crossBC);

      Vector3 crossCB = (nowTrngB - nowTrngC).Cross(pos - nowTrngC);
      Vector3 crossCA = (pos - nowTrngC).Cross(nowTrngA - nowTrngC);
      dotC = crossCB.Dot(crossCA);

      if (dotA > 0.0f && dotB > 0.0f && dotC > 0.0f) // 모두 양수면 삼각형 안에 있는것
      {
         m_Triangles_v[i]->a = nowTrngA;
         m_Triangles_v[i]->b = nowTrngB;
         m_Triangles_v[i]->c = nowTrngC;

         m_IsInTriangle = true;

         return m_Triangles_v[i]; // i번째 삼각형 안에 개미가 존재
      }
   }

   m_IsInTriangle = false; // 삼각형을 다 돌았는데 없다... = 밖이다

   return nullptr; // 실패 음수로 처리
}

void Navigation::CreateVBIBOfTriangle(Triangle* triangle)
{
   /////////////////////////// triangle vertex ///////////////////
   PosColor _vertices[3];
   _vertices[0].Pos = triangle->a;
   _vertices[1].Pos = triangle->b;
   _vertices[2].Pos = triangle->c;
   _vertices[0].Color = Vector4((const float*)&Colors::DarkGray);
   _vertices[1].Color = Vector4((const float*)&Colors::DarkGray);
   _vertices[2].Color = Vector4((const float*)&Colors::DarkGray);

   D3D11_BUFFER_DESC _vbdTriangle;
   _vbdTriangle.Usage = D3D11_USAGE_IMMUTABLE;
   _vbdTriangle.ByteWidth = sizeof(PosColor) * 3;
   _vbdTriangle.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   _vbdTriangle.CPUAccessFlags = 0;
   _vbdTriangle.MiscFlags = 0;
   _vbdTriangle.StructureByteStride = 0;
   D3D11_SUBRESOURCE_DATA vinitDataTriangle;
   vinitDataTriangle.pSysMem = &_vertices[0];
   HR(DXDevice::g_Device->CreateBuffer(&_vbdTriangle, &vinitDataTriangle, &triangle->pVBTriangle));

   /////////////////////////////// triangle index /////////////////
   UINT _indices[3];
   _indices[0] = 0;
   _indices[1] = 1;
   _indices[2] = 2; /// 이거 꺼꾸로해서 반대로 그려지고있었음...

   D3D11_BUFFER_DESC _ibdTriangle;
   _ibdTriangle.Usage = D3D11_USAGE_IMMUTABLE;
   _ibdTriangle.ByteWidth = sizeof(UINT) * 3;
   _ibdTriangle.BindFlags = D3D11_BIND_INDEX_BUFFER;
   _ibdTriangle.CPUAccessFlags = 0;
   _ibdTriangle.MiscFlags = 0;
   _vbdTriangle.StructureByteStride = 0;
   D3D11_SUBRESOURCE_DATA iinitDataTriangle;
   iinitDataTriangle.pSysMem = &_indices[0];
   HR(DXDevice::g_Device->CreateBuffer(&_ibdTriangle, &iinitDataTriangle, &triangle->pIBTriangle));
}

/// 삼각형 안에 점이 있는지 확인하는 방법(dot, cross이용)
// 점 a, b, c로 이뤄진 삼각형과 점 p
// (ab x ap) dot (ap x ac) 의 값이 모든 꼭지점에서 양수면 p는 삼각형 안에있다
Triangle* Navigation::CheckTriangleOnNaviMesh3(Vector3 pos)
{
   float dotA = 0.0f; // a에서의 dot
   float dotB = 0.0f; // b에서의 dot
   float dotC = 0.0f; // c에서의 dot    

   for (int i = 0; i < m_Triangles_v.size(); i++)
   {
      // 삼각형을 메쉬데이터에서 받아온다 
      m_Triangles_v[i]->a = m_NaviMeshData->m_OptimizedVertices_v[m_Triangles_v[i]->aIdx]->m_Pos;
      m_Triangles_v[i]->b = m_NaviMeshData->m_OptimizedVertices_v[m_Triangles_v[i]->bIdx]->m_Pos;
      m_Triangles_v[i]->c = m_NaviMeshData->m_OptimizedVertices_v[m_Triangles_v[i]->cIdx]->m_Pos;

      // 매개변수로 가져온 위치에서 삼각형위로 투영된 위치값을 계산
      Vector3 _posOnTrng = PositionInTriangle(pos, m_Triangles_v[i]);
       
      Vector3 crossAB = (m_Triangles_v[i]->b - m_Triangles_v[i]->a).Cross(_posOnTrng - m_Triangles_v[i]->a);
      Vector3 crossAC =          (_posOnTrng - m_Triangles_v[i]->a).Cross(m_Triangles_v[i]->c - m_Triangles_v[i]->a);
      dotA = crossAB.Dot(crossAC);

      Vector3 crossBA = (m_Triangles_v[i]->a - m_Triangles_v[i]->b).Cross(_posOnTrng - m_Triangles_v[i]->b);
      Vector3 crossBC =          (_posOnTrng - m_Triangles_v[i]->b).Cross(m_Triangles_v[i]->c - m_Triangles_v[i]->b);
      dotB = crossBA.Dot(crossBC);

      Vector3 crossCB = (m_Triangles_v[i]->b - m_Triangles_v[i]->c).Cross(_posOnTrng - m_Triangles_v[i]->c);
      Vector3 crossCA =          (_posOnTrng - m_Triangles_v[i]->c).Cross(m_Triangles_v[i]->a - m_Triangles_v[i]->c);
      dotC = crossCB.Dot(crossCA);

      if (dotA > 0.0f && dotB > 0.0f && dotC > 0.0f) 
      {
         m_IsInTriangle = true; // 모두 양수면 삼각형 안에 있는것

         return m_Triangles_v[i]; // i번째 삼각형 안에 개미가 존재
      }
   }

   m_IsInTriangle = false; // 삼각형을 다 돌았는데 없다 = 밖이다

   return nullptr; 
}

Vector3 Navigation::VectorToKeepInTriangle(Vector3 pos)
{
   Vector3 _vec; // S
   pos; // pOrigin
   Vector3 vDir = {0.0f, -1.0f, 0.0f}; // vDir
   Vector3 N = NormalVectorOfTriangle(m_pNowTriangle); // N
   N.Normalize();

   Vector3 P0 = m_pNowTriangle->a; // P0
   Vector3 vResult = P0 - pos;
   float fResult = vResult.Dot(N);
   float f_t = fResult / (vDir.Dot(N));

   _vec = pos + f_t * vDir;

   return _vec;
}

Vector3 Navigation::PositionInTriangle(Vector3 pos, Triangle* triangle)
{
   // 직선의 방정식에서 : posOnTrng = pos + d*posDir // d를 구한다
   // 평면의 방정식에서 : trngNor dot (p0 - posOnTrng) = 0 
   // posOnTrng를 대입하여 d를 구함
   Vector3 posInTrng; // 삼각형 위의 점 : 결과 값

   Vector3 posDir = { 0.0f, -1.0f, 0.0f }; // pos의 방향 벡터 -Y축 
   Vector3 trngNor = NormalVectorOfTriangle(triangle); // 삼각형의 노말벡터
   Vector3 p0 = triangle->a; // 삼각형위의 한 점

   // 계산과정
   Vector3 vecOnTrng = p0 - pos;
   float distToTrng = (vecOnTrng.Dot(trngNor)) / (posDir.Dot(trngNor));
   posInTrng = pos + distToTrng * posDir;

   return posInTrng;
}

Vector3 Navigation::NormalVectorOfTriangle(Triangle* triangle)
{
   // 삼각형의 노말 계산하기 
   Vector3 vecAB = triangle->b - triangle->a;
   Vector3 vecAC = triangle->c - triangle->a;
   // 삼각형 위 두벡터를 외적하면 노말이 나온다
   Vector3 _normal = vecAC.Cross(vecAB);   
   _normal.Normalize();

   return _normal;
}
