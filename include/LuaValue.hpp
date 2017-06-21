#ifndef LUABZ_LUA_VALUE_HPP
#define LUABZ_LUA_VALUE_HPP
#include <string>
#include "LuaContainer.hpp"
namespace LuaBz
{
	enum class LuaType : int8_t
	{
		Nil, Boolean, Integer, Number, String, Table
	};
	class LuaValue
	{
	public:
		LuaValue();
		LuaValue(const std::string& name, std::nullptr_t data);
		LuaValue(const std::string& name, int data);
		LuaValue(const std::string& name, double data);
		LuaValue(const std::string& name, float data);
		LuaValue(const std::string& name, bool data);
		LuaValue(const std::string& name, const char* data);
		LuaValue(const std::string& name, const std::string& data);
		LuaValue(const std::string& name,const LuaContainer<LuaValue>& data);
		LuaValue(const LuaValue& rhs);
		LuaValue(LuaValue&& rhs);
		LuaValue& operator=(const LuaValue& rhs);
		LuaValue& operator=(LuaValue&& rhs);
		LuaValue& operator=(int rhs);
		LuaValue& operator=(double rhs);
		LuaValue& operator=(float rhs);
		LuaValue& operator=(bool rhs);
		LuaValue& operator=(std::nullptr_t rhs);
		LuaValue& operator=(const std::string& rhs);
		LuaValue& operator=(const char* rhs);
		LuaValue& operator=(const LuaContainer<LuaValue>& rhs);
		~LuaValue();
		const std::string& name() const;
		LuaType type() const;
		//Get
		template <class T>
		T value() const
		{
			static_assert(false, "Use of a type not supported by LuaValue");
		}
		template <>
		int value<int>() const
		{
			return integer();
		}
		template <>
		double value<double>() const
		{
			return number();
		}
		template <>
		bool value<bool>() const
		{
			return boolean();
		}
		template <>
		std::string value<std::string>() const
		{
			return string();
		}
		template <>
		nullptr_t value<std::nullptr_t>() const
		{
			return nil();
		}
		template<>
		LuaContainer<LuaValue> value() const
		{
			return table();
		}
		template <class T>
		const T& value()
		{
			static_assert(false, "Use of a type not supported by LuaValue");
		}
		template <>
		const int& value<int>()
		{
			return m_integer;
		}
		template <>
		const double& value<double>()
		{
			return m_number;
		}
		template <>
		const bool& value<bool>()
		{
			return m_bool;
		}
		template <>
		const std::string& value<std::string>()
		{
			return string();
		}
		template <>
		const nullptr_t& value<std::nullptr_t>()
		{
			return m_nil;
		}
		template<>
		const LuaContainer<LuaValue>& value()
		{
			return table();
		}
	private:
		LuaType m_type;
		std::string m_name;
		union
		{
			std::nullptr_t m_nil;
			int m_integer;
			double m_number;
			bool m_bool;
			std::string m_string;
			LuaContainer <LuaValue> m_table;
		};
		double number() const;
		int integer() const;
		bool boolean() const;
		std::nullptr_t nil() const;
		const std::string& string() const;
		const LuaContainer<LuaValue>& table() const;
		void init(const LuaValue& rhs);
		void copy(const LuaValue& rhs);
		void destroy_complex();
	};
	bool operator < (const LuaValue& lhs, const LuaValue& rhs);
	bool operator ==(const LuaValue& lhs, const LuaValue& rhs);
}
#endif // !LUABZ_LUA_VALUE_HPP
