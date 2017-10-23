#ifndef LUABZ_LUA_TABLE_HPP
#define	LUABZ_LUA_TABLE_HPP

#include <cassert>
#include <vector>
#include <stdexcept>
#include <string>
#include <initializer_list>
namespace LuaBz
{
	//TODO:
	//Inherit from uint8
	enum class LuaType
	{
		Nil, Boolean, Integer, Number, String, Table
	};
	class LuaTable
	{
	public:
		class LuaValue;
	public:
		LuaTable();
		LuaTable(typename std::vector<LuaValue>::size_type count, const LuaValue& value);
		explicit LuaTable(typename std::vector<LuaValue>::size_type count);
		template <class Iter>
		LuaTable(Iter first, Iter last)
			:
			m_values(first, last)
		{

		}
		LuaTable(const std::initializer_list<LuaValue>& init);
		LuaTable(const LuaTable& rhs) = default;
		LuaTable(LuaTable&& rhs) = default;
		LuaTable& operator=(std::initializer_list<LuaValue> ilist);
		LuaTable& operator=(const LuaTable& rhs) = default;
		LuaTable& operator=(LuaTable&& rhs) = default;
		~LuaTable() = default;
		typename std::vector<LuaValue>::reference at(typename std::vector<LuaValue>::size_type pos);
		typename std::vector<LuaValue>::const_reference at(typename std::vector<LuaValue>::size_type pos) const;
		typename std::vector<LuaValue>::reference operator[](typename std::vector<LuaValue>::size_type pos);
		typename std::vector<LuaValue>::const_reference operator[](typename std::vector<LuaValue>::size_type pos) const;
		typename std::vector<LuaValue>::reference front();
		typename std::vector<LuaValue>::const_reference front() const;
		typename std::vector<LuaValue>::reference back();
		typename std::vector<LuaValue>::const_reference back() const;
		typename std::vector<LuaValue>::iterator begin() noexcept;
		typename std::vector<LuaValue>::const_iterator begin() const noexcept;
		typename std::vector<LuaValue>::const_iterator cbegin() const noexcept;
		typename std::vector<LuaValue>::iterator end() noexcept;
		typename std::vector<LuaValue>::const_iterator end() const noexcept;
		typename std::vector<LuaValue>::const_iterator cend() const noexcept;
		typename std::vector<LuaValue>::reverse_iterator rbegin() noexcept;
		typename std::vector<LuaValue>::const_reverse_iterator rbegin() const noexcept;
		typename std::vector<LuaValue>::const_reverse_iterator crbegin() const noexcept;
		typename std::vector<LuaValue>::reverse_iterator rend() noexcept;
		typename std::vector<LuaValue>::const_reverse_iterator rend() const noexcept;
		typename std::vector<LuaValue>::const_reverse_iterator crend() const noexcept;
		bool empty() const noexcept;
		typename std::vector<LuaValue>::size_type size() const noexcept;
		void clear() noexcept;
		void push_back(const LuaValue& val);
		void push_back(LuaValue&& val);
		void pop_back();
		template <class...Args>
		void emplace_back(Args&&... args)
		{
			m_values.emplace_back(std::forward<Args>(args)...);
		}
		void swap(LuaTable& other);
		friend bool operator==(const LuaTable& lhs, const LuaTable& rhs);
		friend bool operator!=(const LuaTable& lhs, const LuaTable& rhs);
		friend bool operator<(const LuaTable& lhs, const LuaTable& rhs);
		friend bool operator<=(const LuaTable& lhs, const LuaTable& rhs);
		friend bool operator>(const LuaTable& lhs, const LuaTable& rhs);
		friend bool operator>=(const LuaTable& lhs, const LuaTable& rhs);
	private:
		std::vector<LuaValue> m_values;
	};
	class LuaTable::LuaValue
	{
	public:
		LuaValue();
		LuaValue(const std::string& name, std::nullptr_t data);
		LuaValue(const std::string& name, int data);
		LuaValue(const std::string& name, double data);
		LuaValue(const std::string& name, float data);
		LuaValue(const std::string& name, bool data);
		LuaValue(const std::string& name, const char* data);
		LuaValue(const std::string& name, const std::string& data);
		LuaValue(const std::string& name, const LuaTable& data);
		LuaValue(const LuaValue& rhs);
		LuaValue(LuaValue&& rhs);
		LuaValue& operator=(const LuaValue& rhs);
		LuaValue& operator=(LuaValue&& rhs);
		LuaValue& operator=(int rhs);
		LuaValue& operator=(double rhs);
		LuaValue& operator=(float rhs);
		LuaValue& operator=(bool rhs);
		LuaValue& operator=(std::nullptr_t rhs);
		LuaValue& operator=(const std::string& rhs);
		LuaValue& operator=(const char* rhs);
		LuaValue& operator=(const LuaTable& rhs);
		~LuaValue();
		const std::string& name() const;
		LuaType type() const;
		//Get
		template <class T>
		T value() const
		{
			//TODO: Find out why this fails with static_assert
			//static_assert(false, "Use of a type not supported by LuaValue");
			assert(false);
		}
		template <class T>
		const T& value()
		{
			//TODO: Find out why this fails with static_assert
			//static_assert(false, "Use of a type not supported by LuaValue");
			assert(false);
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
			LuaTable m_table;
		};
		double number() const;
		int integer() const;
		bool boolean() const;
		std::nullptr_t nil() const;
		const std::string& string() const;
		const LuaTable& table() const;
		void init(const LuaValue& rhs);
		void copy(const LuaValue& rhs);
		void destroy_complex();
	};
	bool operator < (const LuaTable::LuaValue& lhs, const LuaTable::LuaValue& rhs);
	bool operator ==(const LuaTable::LuaValue& lhs, const LuaTable::LuaValue& rhs);
	using LuaValue = LuaTable::LuaValue;

	//Explicit specializations
	template <>
	int LuaTable::LuaValue::value<int>() const
	{
		return integer();
	}
	template <>
	double LuaTable::LuaValue::value<double>() const
	{
		return number();
	}
	template <>
	bool LuaTable::LuaValue::value<bool>() const
	{
		return boolean();
	}
	template <>
	std::string LuaTable::LuaValue::value<std::string>() const
	{
		return string();
	}
	template <>
	nullptr_t LuaTable::LuaValue::value<std::nullptr_t>() const
	{
		return nil();
	}
	template<>
	LuaTable LuaTable::LuaValue::value<LuaTable>() const
	{
		return table();
	}
	template <>
	const int& LuaTable::LuaValue::value<int>()
	{
		return m_integer;
	}
	template <>
	const double& LuaTable::LuaValue::value<double>()
	{
		return m_number;
	}
	template <>
	const bool& LuaTable::LuaValue::value<bool>()
	{
		return m_bool;
	}
	template <>
	const std::string& LuaTable::LuaValue::value<std::string>()
	{
		return string();
	}
	template <>
	const nullptr_t& LuaTable::LuaValue::value<std::nullptr_t>()
	{
		return m_nil;
	}
	template<>
	const LuaTable& LuaTable::LuaValue::value<LuaTable>()
	{
		return table();
	}
}
#endif // ! LUABZ_LUA_TABLE_HPP