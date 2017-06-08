#ifndef LUA_VALUE_HPP
#define LUA_VALUE_HPP
#include <string>
namespace LuaBz
{
	enum class LuaType : short
	{
		Nil, Boolean, Integer, Number, String
	};
	struct LuaValue
	{
	public:
		LuaValue();
		LuaValue(const std::string& name, std::nullptr_t data);
		LuaValue(const std::string& name, int data);
		LuaValue(const std::string& name, double data);
		LuaValue(const std::string& name, float data);
		LuaValue(const std::string& name, bool data);
		LuaValue(const std::string& name, std::string data);
		LuaValue(const LuaValue& rhs);
		LuaValue(LuaValue&& rhs);
		LuaValue& operator=(const LuaValue& rhs);
		LuaValue& operator=(LuaValue&& rhs);
		friend bool operator < (const LuaValue& a, const LuaValue& b);
		friend bool operator ==(const LuaValue& a, const LuaValue& b);
		~LuaValue();
		std::string name() const;
		LuaType type() const;
		double number() const;
		int integer() const;
		bool boolean() const;
		std::nullptr_t nil() const;
		std::string string() const;
	private:
		union
		{
			std::nullptr_t m_nil;
			int m_integer;
			double m_number;
			bool m_bool;
			std::string m_string;
		};
		LuaType m_initialized;
		std::string m_name;
		void copy(const LuaValue& rhs);
	};
}
#endif // !LUA_VALUE_HPP
