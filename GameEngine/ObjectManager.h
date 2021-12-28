#pragma once
#include <vector>


//임시 매니저로 생성자는 아직 알아서 하게 둔다
//신별로 매니저를 두는 등 굳이 싱글턴일 이유가 없을것같다

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

	std::vector<GameObject*> m_Object_V;	//임시 컴포넌트 시스템을 위해 깐다 추후 필히 수정요망
public:
	void SetNowCamera();
private:
	std::vector< Camera*> m_Camera_V;
	Camera* m_pNowCamera;

	//std::vector<std::shared_ptr<ObjectBase>> m_Object_V;
};

