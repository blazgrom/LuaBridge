#include "LuaValue.hpp"
namespace LuaBz
{
	bool operator < (const LuaValue& a, const LuaValue& b)
	{
		if (b.m_initialized == LuaType::Nil)
		{
			return false;
		}
		else if (a.m_initialized == LuaType::Nil)
		{
			return true;
		}
		else
		{
			if (a.m_initialized == LuaType::Boolean)
			{
				if (b.m_initialized == LuaType::Boolean)
				{
					return a.boolean() < b.boolean();
				}
				else
				{
					return false;
				}
			}
			else if (a.m_initialized == LuaType::Integer)
			{
				if (b.m_initialized == LuaType::Integer)
				{
					return a.integer() < b.integer();
				}
				else if (b.m_initialized == LuaType::Number)
				{
					return a.integer() < b.number();
				}
				else
				{
					return false;
				}
			}
			else if (a.m_initialized == LuaType::Number)
			{
				if (b.m_initialized == LuaType::Integer)
				{
					return a.number() < b.integer();
				}
				else if (b.m_initialized == LuaType::Number)
				{
					return a.number() < b.number();
				}
				else
				{
					return false;
				}
			}
			else if (a.m_initialized == LuaType::String)
			{
				if (b.m_initialized == LuaType::String)
				{
					return a.string() < b.string();
				}
				else
				{
					return false;
				}
			}
		}
	}
	bool operator ==(const LuaValue& a, const LuaValue& b)
	{
		return (!(a < b) && !(b < a));
	}
	LuaValue::LuaValue()
		:
		m_initialized{ LuaType::Nil },
		m_name{ "" },
		m_nil{ std::nullptr_t{} }
	{

	}
	LuaValue::LuaValue(const std::string& name, std::nullptr_t data)
		:
		m_initialized{ LuaType::Nil },
		m_name{ name },
		m_nil{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, int data)
		:
		m_initialized{ LuaType::Integer },
		m_name{ name },
		m_integer{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, double data)
		:
		m_initialized{ LuaType::Number },
		m_name{ name },
		m_number{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, float data)
		:
		m_initialized{ LuaType::Number },
		m_name{ name },
		m_number{ static_cast<double>(data) }
	{
	}
	LuaValue::LuaValue(const std::string& name, bool data)
		:
		m_initialized{ LuaType::Boolean },
		m_name{ name },
		m_bool{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, std::string data)
		:
		m_initialized{ LuaType::String },
		m_name{ name },
		m_string{ data }
	{
	}
	LuaValue::LuaValue(const LuaValue& rhs)
		:
		m_initialized{ rhs.m_initialized },
		m_name{ rhs.m_name }
	{
		copy(rhs);
	};
	LuaValue::LuaValue(LuaValue&& rhs)
		:
		m_initialized{ rhs.m_initialized },
		m_name{ rhs.m_name }
	{
		copy(rhs);
	};
	LuaValue& LuaValue::operator=(const LuaValue& rhs)
	{
		if (this == &rhs)
		{
			m_initialized = rhs.m_initialized;
			m_name = rhs.m_name;
			copy(rhs);
		}
		return *this;
	};
	LuaValue& LuaValue::operator=(LuaValue&& rhs)
	{
		m_initialized = rhs.m_initialized;
		m_name = rhs.m_name;
		copy(rhs);
		return *this;
	};
	LuaValue::~LuaValue()
	{
		if (m_initialized == LuaType::String)
			m_string.~basic_string();
	}
	std::string LuaValue::name() const
	{
		return m_name;
	}
	LuaType LuaValue::type() const
	{
		return m_initialized;
	}
	double LuaValue::number() const
	{
		if (m_initialized == LuaType::Number)
			return m_number;
		throw std::runtime_error{ "Number is not initialized" };
	}
	int LuaValue::integer() const
	{
		if (m_initialized == LuaType::Integer)
			return m_integer;
		throw std::runtime_error{ "Integer is not initialized" };
	}
	bool LuaValue::boolean() const
	{
		if (m_initialized == LuaType::Boolean)
			return m_bool;
		throw std::runtime_error{ "Boolean is not initialized" };
	}
	std::nullptr_t LuaValue::nil() const
	{
		if (m_initialized == LuaType::Nil)
			return m_nil;
		throw std::runtime_error{ "Nil is not initialized" };
	}
	std::string LuaValue::string() const
	{
		if (m_initialized == LuaType::String)
			return m_string;
		throw std::runtime_error{ "String is not initialized" };
	}
	void LuaValue::copy(const LuaValue& rhs)//TODO: Decide if this should sbe a member function
	{
		switch (m_initialized)
		{
		case LuaType::Integer:
			m_integer = rhs.m_integer;
			break;
		case LuaType::Nil:
			m_nil = m_nil;
			break;
		case LuaType::Boolean:
			m_bool = rhs.m_bool;
			break;
		case LuaType::Number:
			m_number = rhs.m_number;
			break;
		case LuaType::String:
			m_string.~basic_string();
			new (&m_string)std::string{ m_string };
			break;
		}
	}
}