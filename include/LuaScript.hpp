#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <functional>
#include "LuaHelpers.hpp"
#include "lua.hpp"

#include <iostream>
namespace Lua
{
	class LuaScript
	{
	private:
		lua_State* m_state;
		std::string m_fileName;
		bool m_open;
	public:
		explicit LuaScript(const std::string& file, bool loadStandardLib = true);
		LuaScript(const std::string& file, const std::vector<std::string>& dependencies, bool loadStandardLib = true);
		LuaScript(const std::string& file, const std::string& dependency, bool loadStandardLib = true);
		~LuaScript();
		LuaScript(const LuaScript& rhs) = delete;
		LuaScript& operator=(const LuaScript& rhs) = delete;
		LuaScript(LuaScript&& rhs) = default;
		LuaScript& operator=(LuaScript&& rhs) = default;
		template <class T>
		T get(const std::string& name) const
		{
			if (name.find('.') == std::string::npos)
				getGlobalVariable(name);
			else
				getTableField(name);
			return get_Impl<T>();
		}
		template <class T>
		bool set(const std::string& name, T&& val) const
		{
			if (name.find('.') == std::string::npos)
				return setGlobalVariable(name, val);
			else
				return setTableField(name, val);
		}
		template <class... R, class... Args>
		std::tuple<R...> call(const LuaFunction<R...>& f, Args&&... args) const
		{
			loadFunction(f.name);
			int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
			call_Impl(argumentsCount, f.resultCount, f.name);
			return getFunctionResult<R ...>(f.resultCount);
		}
		template <class...Args>
		void call(const LuaFunction<void>& f, Args&&... args) const
		{
			loadFunction(f.name);
			int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
			call_Impl(argumentsCount, f.resultCount, f.name);
		}
		template <class... T>
		std::tuple<T...> call(const LuaFunction<T...>& f) const
		{
			loadFunction(f.name);
			call_Impl(0, f.resultCount, f.name);
			return getFunctionResult<T...>(f.resultCount);
		}
		void call(const LuaFunction<void>& f) const
		{
			loadFunction(f.name);
			call_Impl(0, f.resultCount, f.name);
		}
		std::map<std::string, std::string> tableInfo(const std::string& table) const;
		void open(const std::string& file);
		void close() noexcept;
		bool change(const std::string& newFile) noexcept;
		void run(std::string luaCode);
	private:
		void initialize(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void loadFile(const std::string& name) const;
		void loadFunction(const std::string& name) const;
		void call_Impl(int inputCount, int outputCount, const std::string& name) const;
		void pop(int count = 1) const;
		void error(const std::string& message, bool popStack = false) const;
		//Pushes on top of the stack a specific field from a the table with the specified index
		void loadTableField(const std::string& field, int tableIndex = -1) const;
		//Call predicate for every entry of a table,assumes that the table is on top of the stack
		//the predicate should remove the value from stack's top
		void iterateTable(std::function<void(const std::string&)> p) const;
		LuaTable createLuaTable() const;
		//Retrieve variable from the top of the stack
		template <class T>
		T get_Impl(int stackIndex = -1) const
		{
			if (!lua_istable(m_state, -1))
			{
				/*
				What we want here is just the name of the type thus we remove any references and cv qualifiers
				*/
				std::string startText = "The variable you are trying to get is not a table, thus cannot be converted to a variable of type ";
				std::string endText = typeid(typename std::decay<T>::type).name();
				error(startText + endText);
			}
			return T{ createLuaTable() };
		}
		template <>
		double get_Impl<double>(int stackIndex) const
		{
			auto r = static_cast<double>(lua_tonumber(m_state, stackIndex));
			pop();
			return r;
		}
		template <>
		float get_Impl<float>(int stackIndex) const
		{
			auto r = static_cast<float>(get_Impl<double>(stackIndex));
			pop();
			return r;
		}
		template <>
		int get_Impl<int>(int stackIndex) const
		{
			auto r = static_cast<int>(lua_tointeger(m_state, stackIndex));
			pop();
			return r;
		}
		template<>
		std::string get_Impl<std::string>(int stackIndex) const
		{
			size_t strLength = 0;
			const char* str = lua_tolstring(m_state, stackIndex, &strLength);
			std::string r{ str, strLength };
			pop();
			return r;
		}
		template <>
		bool  get_Impl<bool>(int stackIndex) const
		{
			auto  r = lua_toboolean(m_state, stackIndex) != 0;
			pop();
			return r;
		}
		template <class T>
		void push(const T& val) const
		{
			lua_newtable(m_state);
			auto table = static_cast<LuaTable>(val);
			for (const auto& element : table.values)
			{
				push(element.name());
				switch (element.type())
				{
				case LuaType::Boolean:
					push(element.boolean());
					break;
				case LuaType::Double:
					push(element.number());
					break;
				case LuaType::Integer:
					push(element.integer());
					break;
				case LuaType::Nil:
					push(element.nil());
					break;
				case LuaType::String:
					push(element.string());
					break;
				}
				lua_settable(m_state, -3); //automatically pops [key,value] 
			}
		}
		template <>
		void push<std::nullptr_t>(const std::nullptr_t& val) const
		{
			lua_pushnil(m_state);
		}
		template <>
		void push<bool>(const bool& val) const
		{
			lua_pushboolean(m_state, val);
		}
		template<>
		void push<std::string>(const std::string& val) const
		{
			lua_pushlstring(m_state, val.c_str(), val.size());
		}
		template <>
		void push<int>(const int& val) const
		{
			lua_pushinteger(m_state, val);
		}
		template<>
		void push<double>(const double& val) const
		{
			lua_pushnumber(m_state, val);
		}
		template<>
		void push<float>(const float& val) const
		{
			push(static_cast<double>(val));
		}
		template <class... Args>
		std::tuple<Args ...> getFunctionResult(unsigned int count) const
		{
			return { getFunctionResult_Impl<Args>(count--) ... };
		}
		template <class T>
		T getFunctionResult_Impl(int index) const
		{
			if (index != 0)
				return  get_Impl<T>(index*-1);
			else
				throw std::runtime_error("You cannot get return values, because there are none");
		}
		template <class T, class... Args>
		int setFunctionParamaters(T&& value, Args&&... args) const
		{
			push(value);
			return 1 + setFunctionParamaters(args...);
		}
		template <class T>
		int setFunctionParamaters(T&& value) const
		{
			push(value);
			return 1;
		}
		template <class T>
		bool setGlobalVariable(const std::string& name, const T& val) const
		{
			try
			{
				getGlobalVariable(name);
				push(val);
				lua_setglobal(m_state, name.c_str());
				return true;
			}
			catch (const std::runtime_error&)
			{
				return false;
			}
		}
		template <class T>
		bool setTableField(const std::string& name, const T& val) const
		{
			try
			{
				std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
				getGlobalVariable(tableName);
				auto keepProcessing = true;
				while (keepProcessing)
				{
					auto dotPosition = tableField.find_first_of('.');
					if (dotPosition == std::string::npos)
					{
						push<T>(val);
						lua_setfield(m_state, -2, tableField.c_str());
						keepProcessing = false;
					}
					else
					{
						std::string parent = tableField.substr(0, dotPosition);
						tableField = tableField.substr(dotPosition + 1);
						loadTableField(parent);
					}
				}
				return true;
			}
			catch (const std::runtime_error&)
			{
				return false;
			}
		}
		void getGlobalVariable(const std::string& name) const;
		void getTableField(const std::string& name) const;
	};
}
#endif // !LUA_SCRIPT_HPP

