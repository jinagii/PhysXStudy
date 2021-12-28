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
				///map�� ������ ������ ���� �߷��� �ִ´�.
				auto k = data.*D.pointer;

				if constexpr (std::is_unsigned<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					///�������� _S�� �پ��ִ��� Ȯ��
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
				/// ������ �ۺ� ��Ÿ���α׷��� ��� ���ø� �б⸦ �¿�� �뵵�� ����ߴ�. 
				/// ���ø��� �ν��Ͻ�ȭ �Ǹ鼭 �����Ǵ� �ڵ忡 �������� �Ұ����� �κ� �߻�
				/// �ش� ���� false��� �ƿ� �����ϵ��� �ʴ´�.
				/// </summary>
				/// 1�� �迭, std::vector (_S : size ������ �Ѱ������)�����մϴ�.
				if constexpr (std::is_pointer<decltype(k)>())
				{
					std::string _variable = D.name;
					_variable.substr(2, _variable.size());

					///�������� _V�� �پ��ִ��� Ȯ��
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
						//�迭
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

					///�������� _V�� �پ��ִ��� Ȯ��
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
