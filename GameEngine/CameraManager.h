#pragma once

class Camera;

/// 카메라가 여러개일 경우가 있을것임
/// 그래서 매니저를 만들어 두었음

class CameraManager
{
public:
   CameraManager();
   ~CameraManager();

   void UpdateNowCamera(float dTime);
   void SetHWND(HWND hWnd) ;
   void SetNowCamera(std::string name);
   Camera* GetNowCamera() const;
   void AddCamera(Camera* pCamera, std::string name);
   void RemoveAllCamera();

   void SetLensOnResize(int width, int height);
   //void OnMouseDown(HWND hWnd, int x, int y);
   //void OnMouseUp();


private:
   std::unordered_map<std::string, Camera*> m_Camera_UM;
   //std::vector<Camera*> m_Camera_V; // 아직 어떻게 써야할지, 
                                    // 액션카메라나 미리동선이 세팅된 카메라를 저장하고 불러올것 같은데 map으로 저장?
   Camera* m_pNowCamera;

   POINT m_PrevMousePos;
   POINT m_CurrMousePos;

   HWND m_hWnd;
};

