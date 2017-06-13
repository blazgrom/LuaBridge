#ifndef LUABZ_LUA_VALUE_HPP
#define LUABZ_LUA_VALUE_HPP
#include <string>
#include <cstdint>
namespace LuaBz
{
	enum class LuaType : int8_t
	{
		Nil, Boolean, Integer, Number, String
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
		LuaValue(const std::string& name,const std::string& data);
		LuaValue(const LuaValue& rhs);
		LuaValue(LuaValue&& rhs);
		LuaValue& operator=(const LuaValue& rhs);
		LuaValue& operator=(LuaValue&& rhs);
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
		//Set new value
		void value(bool newVal);
		void value(std::nullptr_t newVal);
		void value(int newVal);
		void value(double newVal);
		void value(float newVal);
		void value(const std::string& newVal);
		void value(const char* newVal);
		//Overwrite type and set new type
		void value(bool newVal, LuaType newType);
		void value(std::nullptr_t newVal, LuaType newType);
		void value(int newVal, LuaType newType);
		void value(double newVal, LuaType newType);
		void value(float newVal, LuaType newType);
		void value(const std::string& newVal, LuaType newType);
		void value(const char* newVal, LuaType newType);
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
		};
		double number() const;
		int integer() const;
		bool boolean() const;
		std::nullptr_t nil() const;
		std::string string() const;
		void adjust_type(LuaType newType);
		void init(const LuaValue& rhs);
		void copy(const LuaValue& rhs);
	};
	bool operator < (const LuaValue& lhs, const LuaValue& rhs);
	bool operator ==(const LuaValue& lhs, const LuaValue& rhs);
}
#endif // !LUABZ_LUA_VALUE_HPP
