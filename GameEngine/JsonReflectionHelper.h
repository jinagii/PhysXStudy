#pragma once
#include <type_traits>
#include <json/json.h>

#include <boost/describe.hpp>
#include <boost/mp11.hpp>


//template<typename T>
//bool IsPointer(T t)
//{
//	return 	std::is_pointer<T>::value_type();
//}
//
//template<typename T>
//bool IsArray(T t)
//{
//	return 	std::is_array<T>::value_type();
//}

namespace save
{

	template<class Data,
		class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
		class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access>>
		void WriteValue(Json::Value* value, Data const& data)
	{
		boost::mp11::mp_for_each<Bd>([&](auto D)
			{
				using B = typename decltype(D)::type;
			});
		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				(*value)[typeid(data).name()][D.name] = data.*D.pointer;
			});
	}

	template<class Data,
		class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
		class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access>>
		void WriteValue(Json::Value* value, std::string key, Data const& data)
	{

		boost::mp11::mp_for_each<Bd>([&](auto D)
			{
				using B = typename decltype(D)::type;
			});

		std::map<std::string, unsigned int> _vectorSize;
		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				///map에 백터의 사이즈 값만 추려서 넣는다.
				auto k = data.*D.pointer;

				if constexpr (std::is_unsigned<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					///변수끝에 _S가 붙어있는지 확인
					if (_variable.find("_S") != std::string::npos)
					{
						_vectorSize.insert(std::make_pair(_variable, data.*D.pointer));
					}
				}
			});

		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				auto k = data.*D.pointer;

				/// <summary>
				/// constexpr (C++11)
				/// 난해한 템블릿 메타프로그래밍 대신 템플릿 분기를 태우는 용도로 사용했다. 
				/// 템플릿이 인스턴스화 되면서 생성되는 코드에 컴파일이 불가능한 부분 발생
				/// 해당 조건 false라면 아예 컴파일되지 않는다.
				/// </summary>
				/// 1차 배열, std::vector (_S : size 데이터 넘겨줘야함)지원합니다.
				if constexpr (std::is_pointer<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					///변수끝에 _V가 붙어있는지 확인
					if (_variable.find("_V") != std::string::npos)
					{
						//Vector
						Json::Value _vector;

						_variable = D.name;
						_variable.replace(_variable.size() - 2, _variable.size() - 1, "_S");

						for (int i = 0; i < _vectorSize.at(_variable); i++)
						{
							_vector.append(*(k + i));
						}

						(*value)[key.c_str()][D.name] = _vector;

					}
					else
					{
						//배열
						Json::Value _array;
						for (int i = 0; i < (sizeof(data.*D.pointer) / sizeof(*k)); i++)
						{
							_array.append(*(k + i));
						}

						(*value)[key.c_str()][D.name] = _array;
					}
				}
				else
				{
					(*value)[key.c_str()][D.name] = data.*D.pointer;
				}
			});
	}

	///// <summary>
	///// boost/json ver
	///// </summary>

	//template<class Data,
	//	class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
	//	class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access>>
	//	void WriteValue(boost::json::value* value, std::string key, Data const& data)
	//{
	//	boost::mp11::mp_for_each<Bd>([&](auto D)
	//		{
	//			using B = typename decltype(D)::type;
	//		});
	//	boost::mp11::mp_for_each<Md>([&](auto D)
	//		{
	//			value->as_object()[key.c_str()].as_object()[D.name]== data.*D.pointer;
	//		});
	//}

//	template<class T,
//		class D1 = boost::describe::describe_members<T, boost::describe::mod_any_access>>
//		void tag_invoke(boost::json::value_from_tag const&, boost::json::value& v, T const& t)
//	{
//		auto& obj = v.emplace_object();
//		
//		boost::mp11::mp_for_each<D1>([&](auto D) {
//
//			obj[D.name] = boost::json::value_from(t.*D.pointer);
//
//			});
//	}
//}
	template<class Data,
		class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
		class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access>>
		void ReadValue(Json::Value* value, std::string key, Data& data)
	{
		boost::mp11::mp_for_each<Bd>([&](auto D)
			{
				using B = typename decltype(D)::type;
			});

		boost::mp11::mp_for_each<Md>([&](auto D)
			{
				auto k = data.*D.pointer;

				if constexpr (std::is_pointer<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					///변수끝에 _V가 붙어있는지 확인
					if (_variable.find("_V") != std::string::npos)
					{
						std::string a = typeid(data.*D.pointer).name();
						//Vector
						if (data.*D.pointer != nullptr)
						{
							auto j = *k;
							for (int i = 0; i < (*value)[key.c_str()][D.name].size(); i++)
							{
								k[i] = (*value)[key.c_str()][D.name][i].as<decltype(j)>();
							}

						}
						else
						{

						}

					}
					else
					{
						//Array
						for (int i = 0; i < (sizeof(data.*D.pointer) / sizeof(*k)); i++)
						{
							auto j = *k;
							*(k + i) = (*value)[key.c_str()][D.name][i].as<decltype(j)>();
						}
					}
				}
				else
				{
					data.*D.pointer = (*value)[key.c_str()][D.name].as<decltype(k)>();
				}
			});

		return;
	}

	template<class Data, typename T,
		class Bd = boost::describe::describe_bases<Data, boost::describe::mod_any_access>,
		class Md = boost::describe::describe_members<Data, boost::describe::mod_any_access>>
		T GetDataFromValue(Json::Value* value, std::string key, Data& data, T& member)
	{

	}
}
