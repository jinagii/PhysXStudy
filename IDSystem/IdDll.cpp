#include "pch.h"
#include "IdDll.h"

#include <string>

unsigned int g_Count[10] = { 0, };

_ID_DLL const unsigned int ID::GetIdByType(unsigned int type, int detailType)
{
	//Scene 1/ Object 2/ Component 3/
	std::string _id = std::to_string(type);

	_id = _id
		+ std::to_string(type)
		+ std::to_string((detailType < 0) ? 0 : detailType)
		+ std::to_string(g_Count[type]++);


	//std::string _detail;
	//std::string _cnt;

	//sprintf_s((char*)_detail.c_str(), sizeof(_detail.c_str()), "%02d", (detailType < 0) ? 0 : detailType);
	//sprintf_s((char*)_cnt.c_str(), sizeof(_cnt.c_str()), "%07d", g_Count[type]++);

	//_id = _id
	//	+ _detail
	//	+ _cnt;

	return std::stoul(_id.c_str(), nullptr, 10);
}
