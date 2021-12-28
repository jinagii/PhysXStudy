#include "pch.h"
#include "ResourceLoader.h"

#include "IResourceManager.h"
#include "DirectoryReader.h"

#include "CATrace.h"


std::vector<std::string>* ResourceLoader::m_pModelName_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pAnimName_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pTexture_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pShader_V = nullptr;



ResourceLoader::ResourceLoader() 
	: m_pResourceManager(nullptr)
{

}

ResourceLoader::~ResourceLoader()
{

}

void ResourceLoader::Initialize(std::shared_ptr<IResourceManager> pResource)
{
	m_pResourceManager = pResource;

	m_pModelName_V = new std::vector<std::string>;
	m_pTexture_V = new std::vector<std::string>;
	m_pShader_V = new std::vector<std::string>;

}

void ResourceLoader::LoadResource(std::string& name)
{
	DirectoryReader::InputEntryDir(name);

	HRESULT _hr = S_OK;

	try
	{

		if ((DirectoryReader::GetBinaryNames().size() +
			DirectoryReader::GetTextureNames().size() +
			DirectoryReader::GetShaderNames().size() + 
			DirectoryReader::GetAnimNames().size()) == 0)
		{
			throw std::string("경로가 틀림");
		}

		// GraphicMaterial생성 효율을 위해서 Texture 먼저 처리한다.
		for (std::string& _nowTextureName : DirectoryReader::GetTextureNames())
		{
			m_pResourceManager->LoadTextureFile(_nowTextureName);
			m_pTexture_V->push_back(DirectoryReader::GetFileName(_nowTextureName, true));
		}

		// 이쯤에서 머트리얼을 로드해야함!!

		for (std::string& _nowBinFileName : DirectoryReader::GetBinaryNames())
		{
			std::string _nowName = DirectoryReader::GetFileName(_nowBinFileName, true);
			m_pResourceManager->LoadBinaryFile(_nowBinFileName);
			m_pModelName_V->push_back(_nowName);
		}

		for (std::string& _nowAnimFileName : DirectoryReader::GetAnimNames())
		{
			m_pResourceManager->LoadAnimationFile(_nowAnimFileName);
			m_pAnimName_V = new std::vector<std::string>();
			m_pAnimName_V->push_back(DirectoryReader::GetFileName(_nowAnimFileName, true));
		}

		for (std::string& _nowShaderName : DirectoryReader::GetShaderNames())
		{
			m_pResourceManager->LoadShaderFile(_nowShaderName);
			m_pShader_V->push_back(DirectoryReader::GetFileName(_nowShaderName, true));
		}

	}
	catch (std::string exception)
	{
		CA_TRACE("데이터 입력 잘못된 경로 : %s", exception.c_str());
		PostQuitMessage(-1);
	}


}

std::vector<std::string>* ResourceLoader::GetNodeDataNames()
{
	return m_pModelName_V;
}

std::vector<std::string>* ResourceLoader::GetAnimNames()
{
	return m_pAnimName_V;
}

std::vector<std::string>* ResourceLoader::GetTextureNames()
{
	return m_pTexture_V;
}

std::vector<std::string>* ResourceLoader::GetShaderNames()
{
	return m_pShader_V;
}

