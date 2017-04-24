#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include "LuaFunction.hpp"
#include "lua.hpp"
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
		/*
			Permits the user to retrieve a variable, this variable can be either global one or a table
			entry in a global variable
		*/
		template <typename T>
		T get(const std::string& name) const
		{
			if (name.find('.') == std::string::npos)
			{
				loadGlobal(name);
			}
			else
			{
				std::string field = loadTable(name);
				loadTableField(field);
			}
			auto result = getImpl<T>();
			popStack();
			return result;
		}
		/*
			Sets the value of either a global variable or an entry in a global table
		*/
		template <typename T>
		bool set(const std::string& name, T&& val) const
		{
			if (name.find('.') == std::string::npos)
			{
				return setGlobal(name, val);
			}
			else
			{
				return setField(name, val);
			}
		}
		template <typename... R, typename... Args>
		std::tuple<R...> call(const LuaFunction<R...>& f, Args&&... args) const
		{
			loadFunction(f.name);
			int argumentsCount = loadParams(std::forward<Args>(args)...);
			call_Impl(argumentsCount, f.resultCount, f.name);
			return returnValues<R ...>(f.resultCount);
		}
		template <typename...Args>
		void call(const LuaFunction<void>& f, Args&&... args) const
		{
			loadFunction(f.name);
			int argumentsCount = loadParams(std::forward<Args>(args)...);
			call_Impl(argumentsCount, f.resultCount, f.name);
		}
		template <typename... T>
		std::tuple<T...> call(const LuaFunction<T...>& f) const
		{
			loadFunction(f.name);
			call_Impl(0, f.resultCount, f.name);
			return returnValues<T...>(f.resultCount);
		}
		void call(const LuaFunction<void>& f) const
		{
			loadFunction(f.name);
			call_Impl(0, f.resultCount, f.name);
		}
		std::map<std::string, std::string> tableInfo(const std::string& table) const;
		void openFile(const std::string& name);
		void closeFile() noexcept;
		bool changeFile(const std::string& name) noexcept;
		void run(std::string luaCode);
	private:
		void initialize(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void loadFile(const std::string& name) const;
		void loadFunction(const std::string& name) const;
		void popStack(int count = 1) const;
		void call_Impl(int inputValCount, int outputValCount, const std::string& functionName) const;
		void error(const std::string& message) const;
		void iterateTable(std::function<void(const std::string&)> process) const;
		std::string loadTable(std::string name) const;
		void loadTableField(std::string field) const;
		void loadGlobal(const std::string& name) const;
		void getTableField(const std::string& name, int index) const;
		std::map<std::string, std::string > createValuesMap() const;
		//Templates
		template <typename T, typename... Args>
		int loadParams(T&& value, Args&&... args) const
		{
			push(value);
			return 1 + loadParams(args...);
		}
		template <typename T>
		int loadParams(T&& value) const
		{
			push(value);
			return 1;
		}
		template <typename T>
		T getImpl(int stackIndex = -1) const
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
			return T{ createValuesMap() };
		}
		template <>
		double getImpl<double>(int stackIndex) const
		{
			return  static_cast<double>(lua_tonumber(m_state, stackIndex));
		}
		template <>
		float getImpl<float>(int stackIndex) const
		{
			return static_cast<float>(getImpl<double>(stackIndex));
		}
		template <>
		int getImpl<int>(int stackIndex) const
		{
			return  static_cast<int>(lua_tointeger(m_state, stackIndex));
		}
		template<>
		std::string getImpl<std::string>(int stackIndex) const
		{
			size_t strLength = 0;
			const char* str = lua_tolstring(m_state, stackIndex, &strLength);
			return std::string(str, strLength);
		}
		template <>
		bool  getImpl<bool>(int stackIndex) const
		{
			return lua_toboolean(m_state, stackIndex) != 0;
		}
		template <typename T>
		void push(const T& val) const
		{
			lua_newtable(m_state);
			for (const auto& element : val.luaUnpack())
			{
				push(element.first);
				push(element.second);
				lua_settable(m_state, -3); //automatically pops [key,value] 
			}
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
		template <typename... Args>
		std::tuple<Args ...> returnValues(unsigned int count) const
		{
			const unsigned int popCount = count;
			std::tuple<Args ...> result{ returnValue<Args>(count--) ... };
			popStack(popCount);
			return result;
		}
		template <typename T>
		T returnValue(int index) const
		{
			if (index != 0)
			{
				return  getImpl<T>(index*-1);
			}
			else
			{
				throw std::runtime_error("You cannot get return values, because there are none");
			}
		}
		template <typename T>
		bool setGlobal(const std::string& name, const T& val) const
		{
			try
			{
				loadGlobal(name);
				push(val);
				lua_setglobal(m_state, name.c_str());
				return true;
			}
			catch (const std::runtime_error& e)
			{
				return false;
			}
		}
		template <typename T>
		bool setField(const std::string& name, const T& val) const
		{
			try
			{
				std::string field = loadTable(name);
				auto keepProcessing = true;
				while (keepProcessing)
				{
					auto dotPosition = field.find_first_of('.');
					if (dotPosition == std::string::npos)
					{
						push<T>(val);
						lua_setfield(m_state, -2, field.c_str());
						keepProcessing = false;
					}
					else
					{
						std::string parent = field.substr(0, dotPosition);
						field = field.substr(dotPosition + 1);
						getTableField(parent, -1);
					}
				}
				return true;
			}
			catch (const std::runtime_error& e)
			{
				return false;
			}
		}
	};
#endif // !LUA_SCRIPT_HPP
}
