#pragma once

#include <memory>

__interface IRenderer;
__interface IResourceManager;

class InterfaceManager
{
public:
	InterfaceManager();
	~InterfaceManager();

	std::shared_ptr<IRenderer> CreateRenderer();
	std::shared_ptr<IResourceManager> CreateResourceManager();
private:

};

