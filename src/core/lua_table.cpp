#include <cassert>
#include <cmath>
#include "core/lua_table.hpp"
#include "core/lua_type.hpp"
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
		//lua_typesX < Nil -> false, because it's not possible to compare types with nil in Lua
		//Nil < lua_typesX -> true, see lua_typesX < Nil. True because we have inverted the position of Nil
		//lua_typesX < lua_typesX -> compare type if possible, if not possible returns false
		if (rhs.type() == lua_types::Nil)
		{
			return false;
		}
		else if (lhs.type() == lua_types::Nil)
		{
			return true;
		}
		else
		{
			if (lhs.type() == lua_types::Integer)
			{
				if (rhs.type() == lua_types::Integer)
				{
					return lhs.get<lua_t::integer>() < rhs.get<lua_t::integer>();
				}
				else if (rhs.type() == lua_types::Number)
				{
					return lhs.get<lua_t::number>() < rhs.get<lua_t::number>();
				}
			}
			else if (lhs.type() == lua_types::Number)
			{
				if (rhs.type() == lua_types::Integer)
				{
					return lhs.get<lua_t::number>() < rhs.get<lua_t::integer>();
				}
				else if (rhs.type() == lua_types::Number)
				{
					return lhs.get<lua_t::number>() < rhs.get<lua_t::number>();
				}
			}
			else if (lhs.type() == lua_types::String)
			{
				if (rhs.type() == lua_types::String)
				{
					return lhs.get<lua_t::string>() < rhs.get<lua_t::string>();
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
			case lua_types::Integer:
				return lhs.get<lua_t::integer>() == rhs.get<lua_t::integer>();
				break;
			case lua_types::Nil:
				return lhs.get<lua_t::nil>() == rhs.get<lua_t::nil>();
				break;
			case lua_types::Boolean:
				return lhs.get<lua_t::boolean>() && rhs.get<lua_t::boolean>();
				break;
			case lua_types::Number:
			{
				double threshold = (0.1) / 10000000000;
				auto absoluteDifference = std::fabs(lhs.get<lua_t::number>() - rhs.get<lua_t::number>());
				return absoluteDifference <= threshold;
			}
			break;
			case lua_types::String:
				return lhs.get<lua_t::string>() == rhs.get<lua_t::string>();
				break;
			case lua_types::Table:
			{
				if (lhs.get<lua_table>().size() != rhs.get<lua_table>().size())
				{
					return false;
				}
				for (const auto& lhsValue : lhs.get<lua_table>())
				{
					for (const auto& rhsValue : rhs.get<lua_table>())
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
		m_type{ lua_types::Nil },
		m_name{ "" },
		m_nil{ lua_t::nil{} }
	{

	}
	lua_value::lua_value(const std::string& name, lua_t::nil data)
		:
		m_type{ lua_types::Nil },
		m_name{ name },
		m_nil{ data }
	{
	}
	lua_value::lua_value(const std::string& name, lua_t::integer data)
		:
		m_type{ lua_types::Integer },
		m_name{ name },
		m_integer{ data }
	{
	}
	lua_value::lua_value(const std::string& name, lua_t::number data)
		:
		m_type{ lua_types::Number },
		m_name{ name },
		m_number{ data }
	{
	}
	lua_value::lua_value(const std::string& name, float data)
		:
		lua_value(name,lua_t::number(data))
	{
	}
	lua_value::lua_value(const std::string& name, lua_t::boolean data)
		:
		m_type{ lua_types::Boolean },
		m_name{ name },
		m_bool{ data }
	{
	}
	lua_value::lua_value(const std::string& name, const char* data)
		:
		lua_value(name, lua_t::string(data))
	{
	}
	lua_value::lua_value(const std::string& name, const lua_t::string& data)
		:
		m_type{ lua_types::String },
		m_name{ name },
		m_string{ data }
	{
	}
	lua_value::lua_value(const std::string& name, const lua_table& data)
		:
		m_type{ lua_types::Table },
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
	lua_value& lua_value::operator=(lua_t::integer rhs)
	{
		destroy_complex();
		m_integer = rhs;
		m_type = lua_types::Integer;
		return *this;
	}
	lua_value& lua_value::operator=(lua_t::number rhs)
	{
		destroy_complex();
		m_number = rhs;
		m_type = lua_types::Number;
		return *this;
	}
	lua_value& lua_value::operator=(float rhs)
	{
		return operator=(static_cast<lua_t::number>(rhs));
	}
	lua_value& lua_value::operator=(lua_t::boolean rhs)
	{
		destroy_complex();
		m_bool = rhs;
		m_type = lua_types::Boolean;
		return *this;
	}
	lua_value& lua_value::operator=(lua_t::nil rhs)
	{
		destroy_complex();
		m_nil = rhs;
		m_type = lua_types::Nil;
		return *this;
	}
	lua_value& lua_value::operator=(const lua_t::string& rhs)
	{
		destroy_complex();
		new (&m_string)lua_t::string(rhs);
		m_type = lua_types::String;
		return *this;
	}
	lua_value& lua_value::operator=(const char* rhs)
	{
		return operator=(lua_t::string(rhs));
	}
	lua_value& lua_value::operator=(const lua_table& rhs)
	{
		destroy_complex();
		new (&m_table) lua_table(rhs);
		m_type = lua_types::Table;
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
	lua_types lua_value::type() const
	{
		return m_type;
	}
	const lua_t::number& lua_value::get_number() const
	{
		assert(m_type == lua_types::Number);
		return m_number;
	}
	const lua_t::integer& lua_value::get_integer() const
	{
		assert(m_type == lua_types::Integer);
		return m_integer;
	}
	const lua_t::boolean& lua_value::get_boolean() const
	{
		assert(m_type == lua_types::Boolean);
		return m_bool;
	}
	const lua_t::nil& lua_value::get_nil() const
	{
		assert(m_type == lua_types::Nil);
		return m_nil;
	}
	const lua_t::string& lua_value::get_string() const
	{
		assert(m_type == lua_types::String);
		return m_string;
	}
	const lua_table& lua_value::get_table() const
	{
		assert(m_type == lua_types::Table);
		return m_table;
	}
	void lua_value::init(const lua_value& rhs)
	{
		switch (rhs.m_type)
		{
		case lua_types::Integer:
			m_integer = rhs.m_integer;
			break;
		case lua_types::Nil:
			m_nil = rhs.m_nil;
			break;
		case lua_types::Boolean:
			m_bool = rhs.m_bool;
			break;
		case lua_types::Number:
			m_number = rhs.m_number;
			break;
		case lua_types::String:
			new (&m_string)std::string(rhs.m_string);
			break;
		case lua_types::Table:
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
		if (m_type == lua_types::String)
		{
			m_string.~basic_string();
		}
		else if (m_type == lua_types::Table)
		{
			m_table.~lua_table();
		}
	}
}