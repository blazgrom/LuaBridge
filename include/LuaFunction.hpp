#ifndef LUA_FUNCTION_HPP
#define LUA_FUNCTION_HPP
#include <string>
namespace Lua
{
	/*
		TODO:
		-Rename file
		-Add string to LuaValue
	*/
	
	template <typename... T>
	struct LuaFunction
	{
		explicit LuaFunction(const std::string& name)
			:
			name(name),
			resultCount(sizeof... (T))
		{

		}
		std::string name;
		unsigned int resultCount;
	};
	template <>
	struct LuaFunction<void>
	{
		explicit LuaFunction(const std::string& name)
			:
			name(name),
			resultCount(0)
		{

		}
		std::string name;
		unsigned int resultCount;
	};
	enum class LuaType :short
	{
		Integer, Nil, Boolean, Double
	};
	struct LuaValue
	{
	public:
		LuaValue(const std::string& name, std::nullptr_t data)
			:
			m_data{},
			m_initialized{ LuaType::Nil },
			m_name{name}
		{
			m_data.n = data;
		}
		LuaValue(const std::string& name, int data)
			:
			m_data{},
			m_initialized{ LuaType::Integer },
			m_name{ name }
		{
			m_data.i = data;
		}
		LuaValue(const std::string& name, double data)
			:
			m_data{},
			m_initialized{ LuaType::Double },
			m_name{ name }
		{
			m_data.d = data;
		}
		LuaValue(const std::string& name, bool data)
			:
			m_data{},
			m_initialized{ LuaType::Boolean },
			m_name{ name }
		{
			m_data.b = data;
		}
		LuaValue(const LuaValue&) = default;
		LuaValue(LuaValue&&) = default;
		LuaValue& operator=(const LuaValue&) = default;
		LuaValue& operator=(LuaValue&&) = default;
		~LuaValue() = default;
		std::string name() const
		{
			return m_name;
		}
		LuaType type() const
		{
			return m_initialized;
		}
		double number() const
		{
			if (m_initialized == LuaType::Double)
				return m_data.d;
			throw std::runtime_error{"Double is not initialized"};
		}
		int integer() const
		{
			if (m_initialized == LuaType::Integer)
				return m_data.i;
			throw std::runtime_error{ "Int is not initialized" };
		}
		bool boolean() const
		{
			if (m_initialized == LuaType::Boolean)
				return m_data.b;
			throw std::runtime_error{ "Bool is not initialized" };
		}
		std::nullptr_t nil() const
		{
			if (m_initialized == LuaType::Nil)
				return m_data.n;
			throw std::runtime_error{ "Nil is not inialized" };
		}
	private:
		union 
		{
			std::nullptr_t n; //nil
			int i;
			double d;
			bool b;
		} m_data;
		LuaType m_initialized;
		std::string m_name;
	};
	struct LuaTable
	{
		std::vector<LuaValue> values;
	};
}
#endif // !LUA_FUNCTION_HPP
