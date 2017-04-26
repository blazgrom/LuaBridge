#ifndef LUA_FUNCTION_HPP
#define LUA_FUNCTION_HPP
#include <string>
namespace Lua
{
	enum class LuaType :short
	{
		Integer, Nil, Boolean, Double, String
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
			m_data{},
			m_initialized{ LuaType::Nil },
			m_name{ name }
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
		LuaValue(const std::string& name, std::string data)
			:
			m_data{},
			m_initialized{ LuaType::String },
			m_name{ name }
		{
			m_data.s = data;
		}

		LuaValue(const LuaValue& rhs)
			:
			m_data{ rhs.m_data },
			m_initialized{ rhs.m_initialized },
			m_name{ rhs.m_name }
		{
			switch (m_initialized)
			{
			case Lua::LuaType::Integer:
				m_data.i = rhs.m_data.i;
				break;
			case Lua::LuaType::Nil:
				m_data.n = rhs.m_data.n;
				break;
			case Lua::LuaType::Boolean:
				m_data.b = rhs.m_data.b;
				break;
			case Lua::LuaType::Double:
				m_data.d = rhs.m_data.d;
				break;
			case Lua::LuaType::String:
				m_data.s.~basic_string();
				m_data.s = std::move(rhs.m_data.s);
				break;
			}
		};
		LuaValue(LuaValue&& rhs)
			:
			m_data{},
			m_initialized{ rhs.m_initialized },
			m_name{ rhs.m_name }
		{
			switch (m_initialized)
			{
			case Lua::LuaType::Integer:
				m_data.i = rhs.m_data.i;
				break;
			case Lua::LuaType::Nil:
				m_data.n = rhs.m_data.n;
				break;
			case Lua::LuaType::Boolean:
				m_data.b = rhs.m_data.b;
				break;
			case Lua::LuaType::Double:
				m_data.d = rhs.m_data.d;
				break;
			case Lua::LuaType::String:
				m_data.s.~basic_string();
				m_data.s = std::move(rhs.m_data.s);
				break;
			}
		};
		LuaValue& operator=(const LuaValue& rhs)
		{
			if (this == &rhs)
			{
				m_initialized = rhs.m_initialized;
				m_name = rhs.m_name;
				switch (m_initialized)
				{
				case Lua::LuaType::Integer:
					m_data.i = rhs.m_data.i;
					break;
				case Lua::LuaType::Nil:
					m_data.n = rhs.m_data.n;
					break;
				case Lua::LuaType::Boolean:
					m_data.b = rhs.m_data.b;
					break;
				case Lua::LuaType::Double:
					m_data.d = rhs.m_data.d;
					break;
				case Lua::LuaType::String:
					m_data.s.~basic_string();
					m_data.s = std::move(rhs.m_data.s);
					break;
				}
			}
			return *this;
		};
		LuaValue& operator=(LuaValue&& rhs)
		{
			m_initialized = rhs.m_initialized;
			m_name = rhs.m_name;
			switch (m_initialized)
			{
			case Lua::LuaType::Integer:
				m_data.i = rhs.m_data.i;
				break;
			case Lua::LuaType::Nil:
				m_data.n = rhs.m_data.n;
				break;
			case Lua::LuaType::Boolean:
				m_data.b = rhs.m_data.b;
				break;
			case Lua::LuaType::Double:
				m_data.d = rhs.m_data.d;
				break;
			case Lua::LuaType::String:
				m_data.s.~basic_string();
				m_data.s = std::move(rhs.m_data.s);
				break;
			}
			return *this;
		};
		~LuaValue()
		{
			if (m_initialized == Lua::LuaType::String)
				m_data.s.~basic_string();
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
			if (m_initialized == LuaType::Double)
				return m_data.d;
			throw std::runtime_error{ "Double is not initialized" };
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
			throw std::runtime_error{ "Nil is not initialized" };
		}
		std::string string() const
		{
			if (m_initialized == LuaType::String)
				return m_data.s;
			throw std::runtime_error{ "String is not initialized" };
		}
	private:
		union Data
		{
			std::nullptr_t n;
			int i;
			double d;
			bool b;
			std::string s;//Assignment and for s is handled inside LuaValue
			Data() :n{ nullptr }, s{} {};
			~Data() {};
			Data(const Data& rhs)
			{};
			Data& operator=(const Data& rhs)
			{};
		};
		Data m_data;
		LuaType m_initialized;
		std::string m_name;
	};
	struct LuaTable
	{
		std::vector<LuaValue> values;
	};
}
#endif // !LUA_FUNCTION_HPP
