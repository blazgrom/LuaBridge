#ifndef LUABZ_LUA_CONTAINER_HPP
#define LUABZ_LUA_CONTAINER_HPP
#include <vector>
#include <initializer_list>
namespace LuaBz
{
	template<class T>
	class LuaContainer
	{
	public:
		LuaContainer()
			:
			m_values{}
		{

		}
		LuaContainer(typename std::vector<T>::size_type count, const T& value)
			:
			m_values(count, value)
		{

		}
		explicit LuaContainer(typename std::vector<T>::size_type count)
			:
			m_values(count)
		{

		}
		template <class Iter>
		LuaContainer(Iter first, Iter last)
			:
			m_values(first, last)
		{

		}
		LuaContainer(std::initializer_list<T> init)
			:
			m_values(init)
		{

		}
		LuaContainer(const LuaContainer<T>& rhs) = default;
		LuaContainer(LuaContainer<T>&& rhs) = default;
		LuaContainer operator=(std::initializer_list<T> ilist)
		{
			m_values = ilist;
		}
		LuaContainer& operator=(const LuaContainer<T>& rhs) = default;
		LuaContainer& operator=(LuaContainer<T>&& rhs) = default;
		~LuaContainer() = default;
		typename std::vector<T>::reference at(typename std::vector<T>::size_type pos)
		{
			return m_values.at(pos);
		}
		typename std::vector<T>::const_reference at(typename std::vector<T>::size_type pos) const
		{
			return m_values.at(pos);
		}
		typename std::vector<T>::reference operator[](typename std::vector<T>::size_type pos)
		{
			return m_values.at(pos);
		}
		typename std::vector<T>::const_reference operator[](typename std::vector<T>::size_type pos) const
		{
			return m_values.at(pos);
		}
		typename std::vector<T>::reference front()
		{
			return m_values.front();
		}
		typename std::vector<T>::const_reference front() const
		{
			return m_values.front();
		}
		typename std::vector<T>::reference back()
		{
			return m_values.back();
		}
		typename std::vector<T>::const_reference back() const
		{
			return m_values.back();
		}
		typename std::vector<T>::iterator begin() noexcept
		{
			return m_values.begin();
		}
		typename std::vector<T>::const_iterator begin() const noexcept
		{
			return m_values.begin();
		}
		typename std::vector<T>::const_iterator cbegin() const noexcept
		{
			return m_values.cbegin();
		}
		typename std::vector<T>::iterator end() noexcept
		{
			return m_values.end();
		}
		typename std::vector<T>::const_iterator end() const noexcept
		{
			return m_values.end();
		}
		typename std::vector<T>::const_iterator cend() const noexcept
		{
			return m_values.cend();
		}
		typename std::vector<T>::reverse_iterator rbegin() noexcept
		{
			return m_values.rbegin();
		}
		typename std::vector<T>::const_reverse_iterator rbegin() const noexcept
		{
			return m_values.rbegin();
		}
		typename std::vector<T>::const_reverse_iterator crbegin() const noexcept
		{
			return m_values.crbegin();
		}
		typename std::vector<T>::reverse_iterator rend() noexcept
		{
			return m_values.rend();
		}
		typename std::vector<T>::const_reverse_iterator rend() const noexcept
		{
			return m_values.rend();
		}
		typename std::vector<T>::const_reverse_iterator crend() const noexcept
		{
			return m_values.crend();
		}
		bool empty() const noexcept
		{
			return m_values.empty();
		}
		typename std::vector<T>::size_type size() const noexcept
		{
			return m_values.size();
		}
		void clear() noexcept
		{
			m_values.clear();
		}
		void push_back(const T& val)
		{
			m_values.push_back(val);
		}
		void push_back(T&& val)
		{
			m_values.push_back(val);
		}
		void pop_back()
		{
			return m_values.pop_back();
		}
		template <class...Args>
		void emplace_back(Args&&... args)
		{
			m_values.emplace_back(std::forward<Args>(args)...);
		}
		void swap(LuaContainer<T>& other)
		{
			m_values.swap(other.m_values);
		}
		friend bool operator==(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator!=(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator<(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator<=(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator>(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator>=(const LuaContainer<T>& a, const LuaContainer<T>& b);
	private:
		std::vector<T> m_values;
	};
	template<class T>
	bool operator==(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.m_values == b.m_values;
	}
	template<class T>
	bool operator!=(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.m_values != b.m_values;
	}
	template<class T>
	bool operator<(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.m_values < b.m_values;
	}
	template<class T>
	bool operator<=(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.m_values <= b.m_values;
	}
	template<class T>
	bool operator>(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.m_values > b.m_values;
	}
	template<class T>
	bool operator>=(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.m_values >= b.m_values;
	}
}
#endif // !LUABZ_LUA_CONTAINER_HPP
