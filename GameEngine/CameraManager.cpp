#include "pch.h"

#include "Camera.h"
#include "Input.h"

#include "CameraManager.h"

CameraManager::CameraManager()
   : m_pNowCamera(nullptr)
   , m_PrevMousePos({ 0, 0 })
   , m_CurrMousePos({ 0, 0 })
   , m_hWnd(nullptr)
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::UpdateNowCamera(float dTime)
{
    /*
   // input Update
   if (GetAsyncKeyState('W') & 0x8000)
   {
      m_pNowCamera->MoveForward(10.0f * dTime);
   }
   if (GetAsyncKeyState('S') & 0x8000)
   {
      m_pNowCamera->MoveForward(-10.0f * dTime);
   }
   if (GetAsyncKeyState('A') & 0x8000)
   {
      m_pNowCamera->MoveSide(-10.0f * dTime);
      //m_pNowCamera->RotateY(1*dTime);
   }
   if (GetAsyncKeyState('D') & 0x8000)
   {
      m_pNowCamera->MoveSide(10.0f * dTime);
   }
   if (GetAsyncKeyState('Q') & 0x8000)
   {
      m_pNowCamera->MoveUpDown(-10.0f * dTime);
   }
   if (GetAsyncKeyState('E') & 0x8000)
   {
      m_pNowCamera->MoveUpDown(10.0f * dTime);
   }

   */
   //m_PrevMousePos = m_CurrMousePos;
   //GetCursorPos(&m_CurrMousePos);
   //ScreenToClient(m_hWnd, &m_CurrMousePos);

   //if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
   //{
   //   OnMouseDown(m_hWnd, m_CurrMousePos.x, m_CurrMousePos.y);
   //}
   //else
   //{
   //   OnMouseUp();
   //}
   // matrix update
   m_pNowCamera->Update(dTime);
}

void CameraManager::SetHWND(HWND hWnd)
{
   m_hWnd = hWnd;
}

void CameraManager::SetNowCamera(string name)
{
   m_pNowCamera = m_Camera_UM.find(name)->second;
}

Camera* CameraManager::GetNowCamera() const
{
   return m_pNowCamera;
}

void CameraManager::AddCamera(Camera* pCamera, string name)
{
   m_Camera_UM.insert({ name, pCamera });
}

void CameraManager::RemoveAllCamera()
{
    m_Camera_UM.clear();
}

void CameraManager::SetLensOnResize(int width, int height)
{
    if (m_pNowCamera != nullptr)
    {
        m_pNowCamera->SetLens(0.25f * 3.1415, static_cast<float>(width) / height, 1.0f, 10000.0f);
    }
}

//void CameraManager::OnMouseDown(HWND hWnd, int x, int y)
//{
//   SetCapture(hWnd);
//
//   float _dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_PrevMousePos.x));
//   float _dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_PrevMousePos.y));
//
//   //m_pNowCamera->Pitch(_dy);
//   //m_pNowCamera->RotateY(_dx);
//}
//
//void CameraManager::OnMouseUp()
//{
//   ReleaseCapture();
//}