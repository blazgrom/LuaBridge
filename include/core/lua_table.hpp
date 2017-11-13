#ifndef LUABZ_LUA_TABLE_HPP
#define	LUABZ_LUA_TABLE_HPP

#include <cassert>
#include <vector>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include "various.hpp"
namespace LuaBz
{
	enum class LuaType : uint8_t
	{
		Nil, Boolean, Integer, Number, String, Table
	};
	class lua_table
	{
	public:
		class lua_value;
	public:
		lua_table();
		lua_table(typename std::vector<lua_value>::size_type count, const lua_value& value);
		explicit lua_table(typename std::vector<lua_value>::size_type count);
		template <class Iter>
		lua_table(Iter first, Iter last)
			:
			m_values(first, last)
		{
	
		}
		lua_table(const std::initializer_list<lua_value>& init);
		lua_table(const lua_table& rhs) = default;
		lua_table(lua_table&& rhs) = default;
		lua_table& operator=(std::initializer_list<lua_value> ilist);
		lua_table& operator=(const lua_table& rhs) = default;
		lua_table& operator=(lua_table&& rhs) = default;
		~lua_table() = default;
		typename std::vector<lua_value>::reference at(typename std::vector<lua_value>::size_type pos);
		typename std::vector<lua_value>::const_reference at(typename std::vector<lua_value>::size_type pos) const;
		typename std::vector<lua_value>::reference operator[](typename std::vector<lua_value>::size_type pos);
		typename std::vector<lua_value>::const_reference operator[](typename std::vector<lua_value>::size_type pos) const;
		typename std::vector<lua_value>::reference front();
		typename std::vector<lua_value>::const_reference front() const;
		typename std::vector<lua_value>::reference back();
		typename std::vector<lua_value>::const_reference back() const;
		typename std::vector<lua_value>::iterator begin() noexcept;
		typename std::vector<lua_value>::const_iterator begin() const noexcept;
		typename std::vector<lua_value>::const_iterator cbegin() const noexcept;
		typename std::vector<lua_value>::iterator end() noexcept;
		typename std::vector<lua_value>::const_iterator end() const noexcept;
		typename std::vector<lua_value>::const_iterator cend() const noexcept;
		typename std::vector<lua_value>::reverse_iterator rbegin() noexcept;
		typename std::vector<lua_value>::const_reverse_iterator rbegin() const noexcept;
		typename std::vector<lua_value>::const_reverse_iterator crbegin() const noexcept;
		typename std::vector<lua_value>::reverse_iterator rend() noexcept;
		typename std::vector<lua_value>::const_reverse_iterator rend() const noexcept;
		typename std::vector<lua_value>::const_reverse_iterator crend() const noexcept;
		bool empty() const noexcept;
		typename std::vector<lua_value>::size_type size() const noexcept;
		void clear() noexcept;
		void push_back(const lua_value& val);
		void push_back(lua_value&& val);
		void pop_back();
		template <class...Args>
		void emplace_back(Args&&... args)
		{
			m_values.emplace_back(std::forward<Args>(args)...);
		}
		void swap(lua_table& other);
		friend bool operator==(const lua_table& lhs, const lua_table& rhs);
		friend bool operator!=(const lua_table& lhs, const lua_table& rhs);
		friend bool operator<(const lua_table& lhs, const lua_table& rhs);
		friend bool operator<=(const lua_table& lhs, const lua_table& rhs);
		friend bool operator>(const lua_table& lhs, const lua_table& rhs);
		friend bool operator>=(const lua_table& lhs, const lua_table& rhs);
	private:
		std::vector<lua_value> m_values;
	};
	class lua_table::lua_value
	{
	public:
		lua_value();
		lua_value(const std::string& name, std::nullptr_t data);
		lua_value(const std::string& name, int data);
		lua_value(const std::string& name, double data);
		lua_value(const std::string& name, float data);
		lua_value(const std::string& name, bool data);
		lua_value(const std::string& name, const char* data);
		lua_value(const std::string& name, const std::string& data);
		lua_value(const std::string& name, const lua_table& data);
		lua_value(const lua_value& rhs);
		lua_value(lua_value&& rhs);
		lua_value& operator=(const lua_value& rhs);
		lua_value& operator=(lua_value&& rhs);
		lua_value& operator=(int rhs);
		lua_value& operator=(double rhs);
		lua_value& operator=(float rhs);
		lua_value& operator=(bool rhs);
		lua_value& operator=(std::nullptr_t rhs);
		lua_value& operator=(const std::string& rhs);
		lua_value& operator=(const char* rhs);
		lua_value& operator=(const lua_table& rhs);
		~lua_value();
		const std::string& name() const;
		LuaType type() const;
		//Get
		template <typename T>
		T value() const
		{
			static_assert(Utils::always_false<T>::value, "Use of a type not supported by lua_value");
		}
		//Set
		template <typename T>
		const T& value()
		{
			static_assert(Utils::always_false<T>::value, "Use of a type not supported by lua_value");
		}
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
			lua_table m_table;
		};
		double number() const;
		int integer() const;
		bool boolean() const;
		std::nullptr_t nil() const;
		const std::string& string() const;
		const lua_table& table() const;
		void init(const lua_value& rhs);
		void copy(const lua_value& rhs);
		void destroy_complex();
	};
	bool operator < (const lua_table::lua_value& lhs, const lua_table::lua_value& rhs);
	bool operator ==(const lua_table::lua_value& lhs, const lua_table::lua_value& rhs);
	using lua_value = lua_table::lua_value;

	//Explicit specializations
	template <>
	inline int lua_table::lua_value::value<int>() const
	{
		return integer();
	}
	template <>
	inline double lua_table::lua_value::value<double>() const
	{
		return number();
	}
	template <>
	inline bool lua_table::lua_value::value<bool>() const
	{
		return boolean();
	}
	template <>
	inline std::string lua_table::lua_value::value<std::string>() const
	{
		return string();
	}
	template <>
	inline std::nullptr_t lua_table::lua_value::value<std::nullptr_t>() const
	{
		return nil();
	}
	template<>
	inline lua_table lua_table::lua_value::value<lua_table>() const
	{
		return table();
	}
	template <>
	inline const int& lua_table::lua_value::value<int>()
	{
		return m_integer;
	}
	template <>
	inline const double& lua_table::lua_value::value<double>()
	{
		return m_number;
	}
	template <>
	inline const bool& lua_table::lua_value::value<bool>()
	{
		return m_bool;
	}
	template <>
	inline const std::string& lua_table::lua_value::value<std::string>()
	{
		return string();
	}
	template <>
	inline const std::nullptr_t& lua_table::lua_value::value<std::nullptr_t>()
	{
		return m_nil;
	}
	template<>
	inline const lua_table& lua_table::lua_value::value<lua_table>()
	{
		return table();
	}
}
#endif // ! LUABZ_LUA_TABLE_HPP