#pragma once
#include <vector>


//�ӽ� �Ŵ����� �����ڴ� ���� �˾Ƽ� �ϰ� �д�
//�ź��� �Ŵ����� �δ� �� ���� �̱����� ������ �����Ͱ���

class GameObject;
class Camera;
class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	GameObject* CreateObject(bool isContinual);

	void StartAllObject();
	void StartDisContinualObject();
	void UpdateAllObject(float dTime);
	void ReleaseAllObject();
	void ReleaseObject();
	void ResetAllObject();
	void SetupParentChild();
	void AddToManager(GameObject* object);

private:
	void AddCameraToManager(GameObject* cameraObject);
	
public:
	GameObject* FindGameObect(const std::string& objectName);
	GameObject* FindGameObect(const unsigned int& objectId);

	std::vector<GameObject*> m_Object_V;	//�ӽ� ������Ʈ �ý����� ���� ��� ���� ���� �������
public:
	void SetNowCamera();
private:
	std::vector< Camera*> m_Camera_V;
	Camera* m_pNowCamera;

	//std::vector<std::shared_ptr<ObjectBase>> m_Object_V;
};

