#pragma once

#include <memory>
__interface IResourceManager;

class ResourceObserver
{
public:
	// ���ο� IResourceManager�� ������ �־���ҵ�?
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) abstract;
	
};

