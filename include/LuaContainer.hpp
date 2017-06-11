#ifndef LUABZ_LUA_CONTAINER_HPP
#define LUABZ_LUA_CONTAINER_HPP
#include <vector>
#include <initializer_list>
namespace LuaBz
{
	template<class T>
	struct LuaContainer
	{
		//Special functions
		LuaContainer()
			:
			values{}
		{

		}
		LuaContainer(typename std::vector<T>::size_type count, const T& value)
			:
			values(count, value)
		{

		}
		explicit LuaContainer(typename std::vector<T>::size_type count)
			:
			values(count)
		{

		}
		template <class Iter>
		LuaContainer(Iter first, Iter last)
			:
			values(first, last)
		{

		}
		LuaContainer(std::initializer_list<T> init)
			:
			values(init)
		{

		}
		LuaContainer(const LuaContainer<T>& rhs) = default;
		LuaContainer(LuaContainer<T>&& rhs) = default;
		LuaContainer operator=(std::initializer_list<T> ilist)
		{
			values = ilist;
		}
		LuaContainer& operator=(const LuaContainer<T>& rhs) = default;
		LuaContainer& operator=(LuaContainer<T>&& rhs) = default;
		~LuaContainer() = default;
		//Element access
		typename std::vector<T>::reference at(typename std::vector<T>::size_type pos)
		{
			return values.at(pos);
		}
		typename std::vector<T>::const_reference at(typename std::vector<T>::size_type pos) const
		{
			return values.at(pos);
		}
		typename std::vector<T>::reference operator[](typename std::vector<T>::size_type pos)
		{
			return values.at(pos);
		}
		typename std::vector<T>::const_reference operator[](typename std::vector<T>::size_type pos) const
		{
			return values.at(pos);
		}
		typename std::vector<T>::reference front()
		{
			return values.front();
		}
		typename std::vector<T>::const_reference front() const
		{
			return values.front();
		}
		typename std::vector<T>::reference back()
		{
			return values.back();
		}
		typename std::vector<T>::const_reference back() const
		{
			return values.back();
		}
		//Iterators
		typename std::vector<T>::iterator begin() noexcept
		{
			return values.begin();
		}
		typename std::vector<T>::const_iterator begin() const noexcept
		{
			return values.begin();
		}
		typename std::vector<T>::const_iterator cbegin() const noexcept
		{
			return values.cbegin();
		}
		typename std::vector<T>::iterator end() noexcept
		{
			return values.end();
		}
		typename std::vector<T>::const_iterator end() const noexcept
		{
			return values.end();
		}
		typename std::vector<T>::const_iterator cend() const noexcept
		{
			return values.cend();
		}
		typename std::vector<T>::reverse_iterator rbegin() noexcept
		{
			return values.rbegin();
		}
		typename std::vector<T>::const_reverse_iterator rbegin() const noexcept
		{
			return values.rbegin();
		}
		typename std::vector<T>::const_reverse_iterator crbegin() const noexcept
		{
			return values.crbegin();
		}
		typename std::vector<T>::reverse_iterator rend() noexcept
		{
			return values.rend();
		}
		typename std::vector<T>::const_reverse_iterator rend() const noexcept
		{
			return values.rend();
		}
		typename std::vector<T>::const_reverse_iterator crend() const noexcept
		{
			return values.crend();
		}
		//Capacity
		bool empty() const noexcept
		{
			return values.empty();
		}
		typename std::vector<T>::size_type size() const noexcept
		{
			return values.size();
		}
		//Modifiers
		void clear() noexcept
		{
			values.clear();
		}
		void push_back(const T& val)
		{
			values.push_back(val);
		}
		void push_back(T&& val)
		{
			values.push_back(val);
		}
		void pop_back()
		{
			return values.pop_back();
		}
		template <class...Args>
		void emplace_back(Args&&... args)
		{
			values.emplace_back(std::forward<Args>(args)...);
		}
		void swap(LuaContainer<T>& other)
		{
			values.swap(other.values);
		}
		//Non member functions
		friend bool operator==(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator!=(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator<(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator<=(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator>(const LuaContainer<T>& a, const LuaContainer<T>& b);
		friend bool operator>=(const LuaContainer<T>& a, const LuaContainer<T>& b);
	private:
		std::vector<T> values;
	};
	template<class T>
	bool operator==(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.values == b.values;
	}
	template<class T>
	bool operator!=(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.values != b.values;
	}
	template<class T>
	bool operator<(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.values < b.values;
	}
	template<class T>
	bool operator<=(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.values <= b.values;
	}
	template<class T>
	bool operator>(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.values > b.values;
	}
	template<class T>
	bool operator>=(const LuaContainer<T>& a, const LuaContainer<T>& b)
	{
		return a.values >= b.values;
	}
}
#endif // !LUABZ_LUA_CONTAINER_HPP
