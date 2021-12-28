#pragma once

class Camera;

/// ī�޶� �������� ��찡 ��������
/// �׷��� �Ŵ����� ����� �ξ���

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
   //std::vector<Camera*> m_Camera_V; // ���� ��� �������, 
                                    // �׼�ī�޶� �̸������� ���õ� ī�޶� �����ϰ� �ҷ��ð� ������ map���� ����?
   Camera* m_pNowCamera;

   POINT m_PrevMousePos;
   POINT m_CurrMousePos;

   HWND m_hWnd;
};

