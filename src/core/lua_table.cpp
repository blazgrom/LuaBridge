#include <cassert>
#include <cmath>
#include "core/lua_table.hpp"
namespace LuaBz 
{
	//lua_table
	lua_table::lua_table()
		:
		m_values{}
	{
	}
	lua_table::lua_table(typename std::vector<lua_value>::size_type count, const lua_value& value)
		:
		m_values(count, value)
	{
	}
	lua_table::lua_table(typename std::vector<lua_value>::size_type count)
		:
		m_values(count)
	{
	}
	lua_table::lua_table(const std::initializer_list<lua_value>& init)
		:
		m_values(init)
	{
	}
	lua_table& lua_table::operator=(std::initializer_list<lua_value> ilist)
	{
		m_values = ilist;
		return *this;
	}
	typename std::vector<lua_value>::reference lua_table::at(typename std::vector<lua_value>::size_type pos)
	{
		return m_values.at(pos);
	}
	typename std::vector<lua_value>::const_reference lua_table::at(typename std::vector<lua_value>::size_type pos) const
	{
		return m_values.at(pos);
	}
	typename std::vector<lua_value>::reference lua_table::operator[](typename std::vector<lua_value>::size_type pos)
	{
		return m_values.at(pos);
	}
	typename std::vector<lua_value>::const_reference lua_table::operator[](typename std::vector<lua_value>::size_type pos) const
	{
		return m_values.at(pos);
	}
	typename std::vector<lua_value>::reference lua_table::front()
	{
		return m_values.front();
	}
	typename std::vector<lua_value>::const_reference lua_table::front() const
	{
		return m_values.front();
	}
	typename std::vector<lua_value>::reference lua_table::back()
	{
		return m_values.back();
	}
	typename std::vector<lua_value>::const_reference lua_table::back() const
	{
		return m_values.back();
	}
	typename std::vector<lua_value>::iterator lua_table::begin() noexcept
	{
		return m_values.begin();
	}
	typename std::vector<lua_value>::const_iterator lua_table::begin() const noexcept
	{
		return m_values.begin();
	}
	typename std::vector<lua_value>::const_iterator lua_table::cbegin() const noexcept
	{
		return m_values.cbegin();
	}
	typename std::vector<lua_value>::iterator lua_table::end() noexcept
	{
		return m_values.end();
	}
	typename std::vector<lua_value>::const_iterator lua_table::end() const noexcept
	{
		return m_values.end();
	}
	typename std::vector<lua_value>::const_iterator lua_table::cend() const noexcept
	{
		return m_values.cend();
	}
	typename std::vector<lua_value>::reverse_iterator lua_table::rbegin() noexcept
	{
		return m_values.rbegin();
	}
	typename std::vector<lua_value>::const_reverse_iterator lua_table::rbegin() const noexcept
	{
		return m_values.rbegin();
	}
	typename std::vector<lua_value>::const_reverse_iterator lua_table::crbegin() const noexcept
	{
		return m_values.crbegin();
	}
	typename std::vector<lua_value>::reverse_iterator lua_table::rend() noexcept
	{
		return m_values.rend();
	}
	typename std::vector<lua_value>::const_reverse_iterator lua_table::rend() const noexcept
	{
		return m_values.rend();
	}
	typename std::vector<lua_value>::const_reverse_iterator lua_table::crend() const noexcept
	{
		return m_values.crend();
	}
	bool lua_table::empty() const noexcept
	{
		return m_values.empty();
	}
	typename std::vector<lua_value>::size_type lua_table::size() const noexcept
	{
		return m_values.size();
	}
	void lua_table::clear() noexcept
	{
		m_values.clear();
	}
	void lua_table::push_back(const lua_value& val)
	{
		m_values.push_back(val);
	}
	void lua_table::push_back(lua_value&& val)
	{
		m_values.push_back(val);
	}
	void lua_table::pop_back()
	{
		return m_values.pop_back();
	}
	void lua_table::swap(lua_table& other)
	{
		m_values.swap(other.m_values);
	}
	bool operator==(const lua_table& lhs, const lua_table& rhs)
	{
		return lhs.m_values == rhs.m_values;
	}
	bool operator!=(const lua_table& lhs, const lua_table& rhs)
	{
		return lhs.m_values != rhs.m_values;
	}
	bool operator<(const lua_table& lhs, const lua_table& rhs)
	{
		return lhs.m_values < rhs.m_values;
	}
	bool operator<=(const lua_table& lhs, const lua_table& rhs)
	{
		return lhs.m_values <= rhs.m_values;
	}
	bool operator>(const lua_table& lhs, const lua_table& rhs)
	{
		return lhs.m_values > rhs.m_values;
	}
	bool operator>=(const lua_table& lhs, const lua_table& rhs)
	{
		return lhs.m_values >= rhs.m_values;
	}
	//lua_value
	bool operator < (const lua_value& lhs, const lua_value& rhs)
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
	bool operator ==(const lua_value& lhs, const lua_value& rhs)
	{
		//Note:
		//Two lua_values compare equal if they have the save type and the value they hold is the same
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
				if (lhs.value<lua_table>().size() != rhs.value<lua_table>().size())
				{
					return false;
				}
				for (const auto& lhsValue : lhs.value<lua_table>())
				{
					for (const auto& rhsValue : rhs.value<lua_table>())
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
	lua_value::lua_value()
		:
		m_type{ LuaType::Nil },
		m_name{ "" },
		m_nil{ std::nullptr_t{} }
	{

	}
	lua_value::lua_value(const std::string& name, std::nullptr_t data)
		:
		m_type{ LuaType::Nil },
		m_name{ name },
		m_nil{ data }
	{
	}
	lua_value::lua_value(const std::string& name, int data)
		:
		m_type{ LuaType::Integer },
		m_name{ name },
		m_integer{ data }
	{
	}
	lua_value::lua_value(const std::string& name, double data)
		:
		m_type{ LuaType::Number },
		m_name{ name },
		m_number{ data }
	{
	}
	lua_value::lua_value(const std::string& name, float data)
		:
		m_type{ LuaType::Number },
		m_name{ name },
		m_number{ static_cast<double>(data) }
	{
	}
	lua_value::lua_value(const std::string& name, bool data)
		:
		m_type{ LuaType::Boolean },
		m_name{ name },
		m_bool{ data }
	{
	}
	lua_value::lua_value(const std::string& name, const char* data)
		:
		lua_value(name, std::string(data))
	{
	}
	lua_value::lua_value(const std::string& name, const std::string& data)
		:
		m_type{ LuaType::String },
		m_name{ name },
		m_string{ data }
	{
	}
	lua_value::lua_value(const std::string& name, const lua_table& data)
		:
		m_type{ LuaType::Table },
		m_name{ name },
		m_table{ data }
	{

	}
	lua_value::lua_value(const lua_value& rhs)
		:
		m_type{ rhs.m_type },
		m_name{ rhs.m_name }
	{
		init(rhs);
	}
	lua_value::lua_value(lua_value&& rhs)
		:
		m_type{ rhs.m_type },
		m_name{ rhs.m_name }
	{
		init(rhs);
	}
	lua_value& lua_value::operator=(const lua_value& rhs)
	{
		if (this != &rhs)
		{
			copy(rhs);
		}
		return *this;
	}
	lua_value& lua_value::operator=(lua_value&& rhs)
	{
		copy(rhs);
		return *this;
	}
	lua_value& lua_value::operator=(int rhs)
	{
		destroy_complex();
		m_integer = rhs;
		m_type = LuaType::Integer;
		return *this;
	}
	lua_value& lua_value::operator=(double rhs)
	{
		destroy_complex();
		m_number = rhs;
		m_type = LuaType::Number;
		return *this;
	}
	lua_value& lua_value::operator=(float rhs)
	{
		return operator=(static_cast<double>(rhs));
	}
	lua_value& lua_value::operator=(bool rhs)
	{
		destroy_complex();
		m_bool = rhs;
		m_type = LuaType::Boolean;
		return *this;
	}
	lua_value& lua_value::operator=(std::nullptr_t rhs)
	{
		destroy_complex();
		m_nil = rhs;
		m_type = LuaType::Nil;
		return *this;
	}
	lua_value& lua_value::operator=(const std::string& rhs)
	{
		destroy_complex();
		new (&m_string)std::string(rhs);
		m_type = LuaType::String;
		return *this;
	}
	lua_value& lua_value::operator=(const char* rhs)
	{
		return operator=(std::string(rhs));
	}
	lua_value& lua_value::operator=(const lua_table& rhs)
	{
		destroy_complex();
		new (&m_table) lua_table(rhs);
		m_type = LuaType::Table;
		return *this;
	}
	lua_value::~lua_value()
	{
		destroy_complex();
	}
	const std::string& lua_value::name() const
	{
		return m_name;
	}
	LuaType lua_value::type() const
	{
		return m_type;
	}
	double lua_value::number() const
	{
		if (m_type == LuaType::Number)
			return m_number;
		throw std::logic_error{ "Number is not initialized" };
	}
	int lua_value::integer() const
	{
		if (m_type == LuaType::Integer)
			return m_integer;
		throw std::logic_error{ "Integer is not initialized" };
	}
	bool lua_value::boolean() const
	{
		if (m_type == LuaType::Boolean)
			return m_bool;
		throw std::logic_error{ "Boolean is not initialized" };
	}
	std::nullptr_t lua_value::nil() const
	{
		if (m_type == LuaType::Nil)
			return m_nil;
		throw std::logic_error{ "Nil is not initialized" };
	}
	const std::string& lua_value::string() const
	{
		if (m_type == LuaType::String)
			return m_string;
		throw std::logic_error{ "String is not initialized" };
	}
	const lua_table& lua_value::table() const
	{
		if (m_type == LuaType::Table)
			return m_table;
		throw std::logic_error{ "Table is not initialized" };
	}
	void lua_value::init(const lua_value& rhs)
	{
		switch (rhs.m_type)
		{
		case LuaType::Integer:
			m_integer = rhs.m_integer;
			break;
		case LuaType::Nil:
			m_nil = rhs.m_nil;
			break;
		case LuaType::Boolean:
			m_bool = rhs.m_bool;
			break;
		case LuaType::Number:
			m_number = rhs.m_number;
			break;
		case LuaType::String:
			new (&m_string)std::string(rhs.m_string);
			break;
		case LuaType::Table:
			new (&m_table) lua_table(rhs.m_table);
			break;
		default:
			assert(false);
			break;
		}
	}
	void lua_value::copy(const lua_value& rhs)
	{
		destroy_complex();
		init(rhs);
		m_type = rhs.m_type;
	}
	void lua_value::destroy_complex()
	{
		if (m_type == LuaType::String)
		{
			m_string.~basic_string();
		}
		else if (m_type == LuaType::Table)
		{
			m_table.~lua_table();
		}
	}
}