#include <cmath>
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
			case LuaType::Table:
			{
				if (lhs.value<LuaContainer<LuaValue>>().size() != rhs.value<LuaContainer<LuaValue>>().size())
				{
					return false;
				}
				for (const auto& lhsValue : lhs.value<LuaContainer<LuaValue>>())
				{
					for (const auto& rhsValue : rhs.value<LuaContainer<LuaValue>>())
					{
						if (!(lhsValue == rhsValue))
						{
							return false;
						}
					}
				}
				return true;
			}
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
	LuaValue::LuaValue(const std::string& name, const char* data)
		:
		LuaValue(name, std::string(data))
	{
	}
	LuaValue::LuaValue(const std::string& name, const std::string& data)
		:
		m_type{ LuaType::String },
		m_name{ name },
		m_string{ data }
	{
	}
	LuaValue::LuaValue(const std::string& name, const LuaContainer<LuaValue>& data)
		:
		m_type{ LuaType::Table },
		m_name{ name },
		m_table{ data }
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
	LuaValue& LuaValue::operator=(int rhs)
	{
		destroy_complex();
		m_integer = rhs;
		m_type = LuaType::Integer;
		return *this;
	}
	LuaValue& LuaValue::operator=(double rhs)
	{
		destroy_complex();
		m_number = rhs;
		m_type = LuaType::Number;
		return *this;
	}
	LuaValue& LuaValue::operator=(float rhs)
	{
		return operator=(static_cast<double>(rhs));
	}
	LuaValue& LuaValue::operator=(bool rhs)
	{
		destroy_complex();
		m_bool = rhs;
		m_type = LuaType::Boolean;
		return *this;
	}
	LuaValue& LuaValue::operator=(std::nullptr_t rhs)
	{
		destroy_complex();
		m_nil = rhs;
		m_type = LuaType::Nil;
		return *this;
	}
	LuaValue& LuaValue::operator=(const std::string& rhs)
	{
		destroy_complex();
		new (&m_string)std::string(rhs );
		m_type = LuaType::String;
		return *this;
	}
	LuaValue& LuaValue::operator=(const char* rhs)
	{
		return operator=(std::string(rhs));
	}
	LuaValue& LuaValue::operator=(const LuaContainer<LuaValue>& rhs)
	{
		destroy_complex();
		new (&m_table) LuaContainer<LuaValue>(rhs);
		m_type = LuaType::Table;
		return *this;
	}
	LuaValue::~LuaValue()
	{
		destroy_complex();
	}
	const std::string& LuaValue::name() const
	{
		return m_name;
	}
	LuaType LuaValue::type() const
	{
		return m_type;
	}
	double LuaValue::number() const
	{
		if (m_type == LuaType::Number)
			return m_number;
		throw std::logic_error{ "Number is not initialized" };
	}
	int LuaValue::integer() const
	{
		if (m_type == LuaType::Integer)
			return m_integer;
		throw std::logic_error{ "Integer is not initialized" };
	}
	bool LuaValue::boolean() const
	{
		if (m_type == LuaType::Boolean)
			return m_bool;
		throw std::logic_error{ "Boolean is not initialized" };
	}
	std::nullptr_t LuaValue::nil() const
	{
		if (m_type == LuaType::Nil)
			return m_nil;
		throw std::logic_error{ "Nil is not initialized" };
	}
	const std::string& LuaValue::string() const
	{
		if (m_type == LuaType::String)
			return m_string;
		throw std::logic_error{ "String is not initialized" };
	}
	const LuaContainer<LuaValue>& LuaValue::table() const
	{
		if (m_type == LuaType::Table)
			return m_table;
		throw std::logic_error{ "Table is not initialized" };
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
			new (&m_string)std::string( rhs.m_string );
			break;
		case LuaType::Table:
			new (&m_table) LuaContainer<LuaValue>(rhs.m_table);
			break;
		default:
			assert(false);
			break;
		}
	}
	void LuaValue::copy(const LuaValue& rhs)
	{
		destroy_complex();
		init(rhs);
		m_type = rhs.m_type;
	}
	void LuaValue::destroy_complex()
	{
		if (m_type == LuaType::String)
		{
			m_string.~basic_string();
		}
		else if (m_type == LuaType::Table)
		{
			m_table.~LuaContainer();
		}

	}
}