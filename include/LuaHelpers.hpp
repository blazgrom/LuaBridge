#ifndef LUA_FUNCTION_HPP
#define LUA_FUNCTION_HPP
#include <string>
namespace Lua
{
	enum class LuaType :short
	{
		Integer, Nil, Boolean, Number, String
	};
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
	struct LuaValue
	{
	public:
		LuaValue(const std::string& name, std::nullptr_t data)
			:
			m_initialized{ LuaType::Nil },
			m_name{ name },
			m_nil{data}
		{
		}
		LuaValue(const std::string& name, int data)
			:
			m_initialized{ LuaType::Integer },
			m_name{ name },
			m_integer{data}
		{
		}
		LuaValue(const std::string& name, double data)
			:
			m_initialized{ LuaType::Number },
			m_name{ name },
			m_number{data}
		{
		}
		LuaValue(const std::string& name, float data)
			:
			m_initialized{ LuaType::Number },
			m_name{ name },
			m_number{ static_cast<double>(data) }
		{
		}
		LuaValue(const std::string& name, bool data)
			:
			m_initialized{ LuaType::Boolean },
			m_name{ name },
			m_bool{data}
		{
		}
		LuaValue(const std::string& name, std::string data)
			:
			m_initialized{ LuaType::String },
			m_name{ name },
			m_string{data}
		{
		}
		LuaValue(const LuaValue& rhs)
			:
			m_initialized{ rhs.m_initialized },
			m_name{ rhs.m_name }
		{
			copy(rhs);
		};
		LuaValue(LuaValue&& rhs)
			:
			m_initialized{ rhs.m_initialized },
			m_name{ rhs.m_name }
		{
			copy(rhs);
		};
		LuaValue& operator=(const LuaValue& rhs)
		{
			if (this == &rhs)
			{
				m_initialized = rhs.m_initialized;
				m_name = rhs.m_name;
				copy(rhs);
			}
			return *this;
		};
		LuaValue& operator=(LuaValue&& rhs)
		{
			m_initialized = rhs.m_initialized;
			m_name = rhs.m_name;
			copy(rhs);
			return *this;
		};
		~LuaValue()
		{
			if (m_initialized == Lua::LuaType::String)
				m_string.~basic_string();
		}
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
			if (m_initialized == LuaType::Number)
				return m_number;
			throw std::runtime_error{ "Number is not initialized" };
		}
		int integer() const
		{
			if (m_initialized == LuaType::Integer)
				return m_integer;
			throw std::runtime_error{ "Integer is not initialized" };
		} 
		bool boolean() const
		{
			if (m_initialized == LuaType::Boolean)
				return m_bool;
			throw std::runtime_error{ "Boolean is not initialized" };
		}
		std::nullptr_t nil() const
		{
			if (m_initialized == LuaType::Nil)
				return m_nil;
			throw std::runtime_error{ "Nil is not initialized" };
		}
		std::string string() const
		{
			if (m_initialized == LuaType::String)
				return m_string;
			throw std::runtime_error{ "String is not initialized" };
		}
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
		void copy(const LuaValue& rhs)
		{
			switch (m_initialized)
			{
			case Lua::LuaType::Integer:
				m_integer = rhs.m_integer;
				break;
			case Lua::LuaType::Nil:
				m_nil = m_nil;
				break;
			case Lua::LuaType::Boolean:
				m_bool = rhs.m_bool;
				break;
			case Lua::LuaType::Number:
				m_number = rhs.m_number;
				break;
			case Lua::LuaType::String:
				m_string.~basic_string();
				m_string = rhs.m_string;
				break;
			}
		}
	};
	struct LuaTable
	{
		std::vector<LuaValue> values;
	};
}
#endif // !LUA_FUNCTION_HPP
