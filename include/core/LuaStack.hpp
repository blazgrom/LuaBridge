#ifndef LUABZ_LUA_STACK_HPP
#define LUABZ_LUA_STACK_HPP
#include <vector>
#include <algorithm>
#include <cassert>
#include <string>
#include <functional>
#include "lua.hpp"
#include "LuaTable.hpp"
#include "LuaError.hpp"
#include "can_represent_value.hpp"
namespace LuaBz
{

	class LuaStack
	{
	public:
		LuaStack(bool loadStd, const std::string& file, std::vector<std::string> dependencies = {})
			:
			m_state{ luaL_newstate() }
		{
			if (loadStd)
				luaL_openlibs(m_state);
			for (const std::string& depend : dependencies)
			{
				run_file(depend);
			}
			run_file(file);
		}
		~LuaStack()
		{
			lua_close(m_state);
		}
		void pop(int count = 1) const;
		void set_top_element(const std::string& name) const;
		template <class T>
		T top_element(bool popTopElement = false) const
		{
			if (popTopElement)
			{
				T result = get<T>(topElement);
				pop();
				return result;
			}
			return get<T>(topElement);
		}
		template <class T>
		void set_element(const std::string& name, T&& value) const
		{
			if (name.find('.') == std::string::npos)
				set_global_variable(name, value);
			else
				set_table_element(name, value);
		}
		template <class T>
		T get_element(int index) const
		{
			auto d = get<T>(index);
			return d;
		}
		template <class T>
		typename std::enable_if<std::is_convertible<T, LuaTable>::value>::type push(T val) const
		{
			auto table = static_cast<LuaTable>(val);
			push(table);
		}
		void push(const LuaTable& val) const;
		void push(lua_CFunction function) const;
		void push(std::nullptr_t) const;
		void push(std::string val) const;
		void push(char val) const;
		void push(bool val) const;
		void push(short val) const;
		void push(int val) const;
		void push(unsigned int val) const;
		void push(long val) const;
		void push(unsigned long val) const;
		void push(long long val) const;
		void push(unsigned long long val) const;
		void push(float val) const;
		void push(double val) const;
		void call_function(int inputCount, int outputCount) const;
		bool is_function(int index = -1) const;
		int size() const;
		void execute(const std::string& code) const;
		void destroy();
		void create(const std::string& file);
	private:
		lua_State* m_state;
		static const int topElement = -1;
		template <class T>
		void set_global_variable(const std::string& name, T&& value) const
		{
				push(value);
				lua_setglobal(m_state, name.c_str());
		}
		template <class T>
		void set_table_element(const std::string& name, const T& val) const
		{
			
				std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
				get_global_variable(tableName);//
				auto keepProcessing = true;
				while (keepProcessing)
				{
					auto dotPosition = tableField.find_first_of('.');
					if (dotPosition == std::string::npos)
					{
						push(val);
						lua_setfield(m_state, -2, tableField.c_str());
						keepProcessing = false;
					}
					else
					{
						std::string parent = tableField.substr(0, dotPosition);
						tableField = tableField.substr(dotPosition + 1);
						load_table_field(parent);
					}
				}
		}
		void run_file(const std::string& name) const;
		void get_global_variable(const std::string& name) const;
		void get_table_element(const std::string& name) const;
		void load_table_field(const std::string& name) const;
		template<class T>
		T get_lua_number(int index, std::true_type) const
		{
			T result = static_cast<T>(lua_tonumber(m_state, index));
			return result;
		}
		template <class T>
		T get_lua_number(int , std::false_type) const
		{
			assert(false);
			return T{};
		}
		template <class T>
		T get_lua_integer(int index, std::true_type) const
		{
			T result = static_cast<T>(lua_tointeger(m_state, index));
			return result;
		}
		template <class T>
		T get_lua_integer(int , std::false_type) const
		{
			assert(false);
			return T{};
		}
		template <class T>
		T get(int index) const
		{
			static_assert(std::is_constructible<T, LuaTable>::value, "Type cannot be constructed from a LuaTable");
			return T{ get<LuaTable>(index)};
		}
	
		LuaTable create_lua_table() const;
		void iterate_table(std::function<void(const std::string&)> callback) const;
	};
	//Explicit specializations
	template<>
	inline LuaTable LuaStack::get<LuaTable>(int index) const
	{
		if (!lua_istable(m_state, index))
		{
			throw LuaError("The type you are trying to retrieve cannot be constructed with a LuaTable");
		}
		return  create_lua_table();
	}
	template<>
	inline std::string LuaStack::get<std::string>(int index) const
	{
		size_t strLength = 0;
		const char* str = lua_tolstring(m_state, index, &strLength);
		std::string r{ str, strLength };
		return r;
	}
	template <>
	inline char LuaStack::get<char>(int index) const
	{
		auto str = get<std::string>(index);
		return str[0];
	}
	template <>
	inline bool LuaStack::get<bool>(int index) const
	{
		return lua_toboolean(m_state, index) != 0;
	}
	template <>
	inline short LuaStack::get<short>(int index) const
	{
		return get_lua_integer<short>(index, Utils::Can_represent_value<lua_Integer, short>{});
	}
	template <>
	inline unsigned short LuaStack::get<unsigned short>(int index) const
	{
		return get_lua_integer<unsigned short>(index, Utils::Can_represent_value<lua_Integer, unsigned short>{});
	}
	template <>
	inline int LuaStack::get<int>(int index) const
	{
		return get_lua_integer<int>(index, Utils::Can_represent_value<lua_Integer, int>{});
	}
	template <>
	inline unsigned int LuaStack::get<unsigned int>(int index) const
	{
		return get_lua_integer<unsigned int>(index, Utils::Can_represent_value<lua_Integer, unsigned int>{});
	}
	template <>
	inline long LuaStack::get<long>(int index) const
	{
		return get_lua_integer<long>(index, Utils::Can_represent_value<lua_Integer, long>{});
	}
	template <>
	inline unsigned long LuaStack::get<unsigned long>(int index) const
	{
		return get_lua_integer<unsigned long>(index, Utils::Can_represent_value<lua_Integer, unsigned long>{});
	}
	template <>
	inline long long LuaStack::get<long long>(int index) const
	{
		return get_lua_integer<long long>(index, Utils::Can_represent_value<lua_Integer, long long>{});
	}
	template <>
	inline unsigned long long LuaStack::get<unsigned long long>(int index) const
	{
		return get_lua_integer<unsigned long long>(index, Utils::Can_represent_value<lua_Integer, unsigned long long>{});
	}
	template <>
	inline double  LuaStack::get<double>(int index) const
	{
		return get_lua_number<double>(index, Utils::Can_represent_value<lua_Number, double>{});
	}
	template <>
	inline float  LuaStack::get<float>(int index) const
	{
		return get_lua_number<float>(index, Utils::Can_represent_value<lua_Number, float>{});
	}
}
#endif // !LUABZ_LUA_STACK_HPP