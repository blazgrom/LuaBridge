#include <cmath>
#include <string>
#include <cassert>
#include <stdexcept>
#include "LuaValue.hpp"
namespace LuaBz
{
	bool operator < (const LuaValue& lhs, const LuaValue& rhs)
	{
		//Note:
		//It emulates as best as it can the logic of Lua
		//LuaTypeX < Nil -> false, because it's not possible to compare types with nil in Lua
		//Nil < LuaTypeX -> true, see LuaTypeX < Nil. True because we have inverted the position of Nil
		//LuaTypeX < LuaTypeX -> compare type if possible, if not possible returns false
		if (rhs.type() == LuaType::Nil)
		{
			return false; 
		}
		else if (lhs.type() == LuaType::Nil)
		{
			return true;
		}
		else
		{
			if (lhs.type() == LuaType::Integer)
			{
				if (rhs.type() == LuaType::Integer)
				{
					return lhs.value<int>() < rhs.value<int>();
				}
				else if (rhs.type() == LuaType::Number)
				{
					return lhs.value<int>() < rhs.value<double>();
				}
			}
			else if (lhs.type() == LuaType::Number)
			{
				if (rhs.type() == LuaType::Integer)
				{
					return lhs.value<double>() < rhs.value<int>();
				}
				else if (rhs.type() == LuaType::Number)
				{
					return lhs.value<double>() < rhs.value<double>();
				}
			}
			else if (lhs.type() == LuaType::String)
			{
				if (rhs.type() == LuaType::String)
				{
					return lhs.value<std::string>() < rhs.value<std::string>();
				}
			}
		}
		return false;
	}
	bool operator ==(const LuaValue& lhs, const LuaValue& rhs)
	{
		//Note:
		//Two LuaValues compare equal if they have the save type and the value they hold is the same
		bool sameType = lhs.type() == rhs.type();
		if (sameType)
		{
			switch (lhs.type())
			{
			case LuaType::Integer:
				return lhs.value<int>() == rhs.value<int>();
				break;
			case LuaType::Nil:
				return lhs.value<std::nullptr_t>() == rhs.value<std::nullptr_t>();
				break;
			case LuaType::Boolean:
				return lhs.value<bool>() && rhs.value<bool>();
				break;
			case LuaType::Number:
				{
					double threshold = (0.1) / 10000000000;
					auto absoluteDifference = std::fabs(lhs.value<double>() - rhs.value<double>());
					return absoluteDifference <= threshold;
				}
				break;
			case LuaType::String:
				return lhs.value<std::string>() == rhs.value<std::string>();
				break;
			default:
				assert(false);
				break;
			}
		}
		return false;
	}
	LuaValue::LuaValue()
		:
		m_type{ LuaType::Nil },
		m_name{ "" },
		m_nil{ std::nullptr_t{} }
	{

	}
	LuaValue::LuaValue(const std::string& name, std::nullptr_t data)
		:
		m_type{ LuaType::Nil },
		m_name{ name },
		m_nil{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, int data)
		:
		m_type{ LuaType::Integer },
		m_name{ name },
		m_integer{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, double data)
		:
		m_type{ LuaType::Number },
		m_name{ name },
		m_number{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, float data)
		:
		m_type{ LuaType::Number },
		m_name{ name },
		m_number{ static_cast<double>(data) }
	{
	}
	LuaValue::LuaValue(const std::string& name, bool data)
		:
		m_type{ LuaType::Boolean },
		m_name{ name },
		m_bool{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, const std::string& data)
		:
		m_type{ LuaType::String },
		m_name{ name },
		m_string{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, const char* data)
		:
		LuaValue(name, std::string(data))
	{
	}
	LuaValue::LuaValue(const LuaValue& rhs)
		:
		m_type{ rhs.m_type },
		m_name{ rhs.m_name }
	{
		init(rhs);
	};
	LuaValue::LuaValue(LuaValue&& rhs)
		:
		m_type{ rhs.m_type },
		m_name{ rhs.m_name }
	{
		init(rhs);
	};
	LuaValue& LuaValue::operator=(const LuaValue& rhs)
	{
		if (this != &rhs)
		{
			copy(rhs);
		}
		return *this;
	};
	LuaValue& LuaValue::operator=(LuaValue&& rhs)
	{
		copy(rhs);
		return *this;
	};
	LuaValue::~LuaValue()
	{
		if (m_type == LuaType::String)
			m_string.~basic_string();
	}
	//Public
	const std::string& LuaValue::name() const
	{
		return m_name;
	}
	LuaType LuaValue::type() const
	{
		return m_type;
	}
	void LuaValue::value(bool newVal)
	{
		if (m_type != LuaType::Boolean)
		{
			throw std::exception("LuaValue does not have value of type bool");
		}
		m_bool = newVal;
	}
	void LuaValue::value(std::nullptr_t newVal)
	{
		if (m_type != LuaType::Nil)
		{
			throw std::exception("LuaValue does not have value of type nil");
		}
		m_nil = newVal;
	}
	void LuaValue::value(int newVal)
	{
		if (m_type != LuaType::Integer)
		{
			throw std::exception("LuaValue does not have value of type integer");
		}
		m_integer = newVal;
	}
	void LuaValue::value(double newVal)
	{
		if (m_type != LuaType::Number)
		{
			throw std::exception("LuaValue does not have value of type number");
		}
		m_number = newVal;
	}
	void LuaValue::value(float newVal)
	{
		value(static_cast<double>(newVal));
	}
	void LuaValue::value(const std::string& newVal)
	{
		if (m_type != LuaType::String)
		{
			throw std::exception("LuaValue does not have value of type string");
		}
		new (&m_string)std::string{ newVal };
	}
	void LuaValue::value(const char* newVal)
	{
		value(std::string(newVal));
	}
	void LuaValue::value(bool newVal, LuaType newType)
	{
		if (newType != LuaType::Boolean)
		{
			throw std::logic_error("LuaType different from the type of the value");
		}
		adjust_type(newType);
		value(newVal);
	}
	void LuaValue::value(std::nullptr_t newVal, LuaType newType)
	{
		if (newType != LuaType::Nil)
		{
			throw std::logic_error("LuaType different from the type of the value");
		}
		adjust_type(newType);
		value(newVal);
	}
	void LuaValue::value(int newVal, LuaType newType)
	{
		if (newType != LuaType::Integer)
		{
			throw std::logic_error("LuaType different from the type of the value");
		}
		adjust_type(newType);
		value(newVal);
	}
	void LuaValue::value(double newVal, LuaType newType)
	{
		if (newType != LuaType::Number)
		{
			throw std::logic_error("LuaType different from the type of the value");
		}
		adjust_type(newType);
		value(newVal);
	}
	void LuaValue::value(float newVal, LuaType newType)
	{
		value(static_cast<double>(newVal));
	}
	void LuaValue::value(const std::string& newVal, LuaType newType)
	{
		if (newType != LuaType::String)
		{
			throw std::logic_error("LuaType different from the type of the value");
		}
		adjust_type(newType);
		new (&m_string)std::string{ newVal };
	}
	void LuaValue::value(const char* newVal, LuaType newType)
	{
		value(std::string(newVal), newType);
	}
	//Private
	double LuaValue::number() const
	{
		if (m_type == LuaType::Number)
			return m_number;
		throw std::runtime_error{ "Number is not initialized" };
	}
	int LuaValue::integer() const
	{
		if (m_type == LuaType::Integer)
			return m_integer;
		throw std::runtime_error{ "Integer is not initialized" };
	}
	bool LuaValue::boolean() const
	{
		if (m_type == LuaType::Boolean)
			return m_bool;
		throw std::runtime_error{ "Boolean is not initialized" };
	}
	std::nullptr_t LuaValue::nil() const
	{
		if (m_type == LuaType::Nil)
			return m_nil;
		throw std::runtime_error{ "Nil is not initialized" };
	}
	std::string LuaValue::string() const
	{
		if (m_type == LuaType::String)
			return m_string;
		throw std::runtime_error{ "String is not initialized" };
	}
	void LuaValue::adjust_type(LuaType newType)
	{
		if (newType != m_type)
		{
			if (m_type == LuaType::String)
			{
				m_string.~basic_string();
			}
			m_type = newType;
		}
	}
	void LuaValue::init(const LuaValue& rhs)
	{
		switch (rhs.m_type)
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
			new (&m_string)std::string{ rhs.m_string };
			break;
		default:
			assert(false);
			break;
		}
	}
	void LuaValue::copy(const LuaValue& rhs)
	{
		if (m_type == LuaType::String)
		{
			m_string.~basic_string();
		}
		init(rhs);
		m_type = rhs.m_type;
	}
}