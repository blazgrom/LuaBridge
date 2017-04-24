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
			auto result = getValue<T>();
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
			checkDataValidity<R ...>(f.resultCount);
			loadFunction(f.name);
			int argumentsCount = loadParams(std::forward<Args>(args)...);
			callFunction(argumentsCount, f.resultCount, f.name);
			return getReturnValues<R ...>(f.resultCount);
		}
		template <typename...Args>
		void call(const LuaFunction<void>& function, Args&&... args) const
		{
			loadFunction(function.name);
			int argumentsCount = loadParams(std::forward<Args>(args)...);
			callFunction(argumentsCount, function.resultCount, function.name);
		}
		template <typename... T>
		std::tuple<T...> call(const LuaFunction<T...>& function) const
		{
			checkDataValidity<T...>(function.resultCount);
			loadFunction(function.name);
			callFunction(0, function.resultCount, function.name);
			return getReturnValues<T...>(function.resultCount);
		}
		void call(const LuaFunction<void>& function) const
		{
			loadFunction(function.name);
			callFunction(0, function.resultCount, function.name);
		}
		std::map<std::string, std::string> getTableInfo(const std::string& name) const;
		void openFile(const std::string& name);
		void closeFile() noexcept;
		bool changeFile(const std::string& name) noexcept;
		void run(std::string luaCode);
	private:
		void initialize(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void loadFile(const std::string& name) const;
		void loadFunction(const std::string& name) const;
		void popStack(int count = 1) const;
		void callFunction(int inputValCount, int outputValCount, const std::string& functionName) const;
		void generateError(const std::string& message) const;
		void iterateTable(std::function<void(const std::string&)> process) const;
		std::string loadTable(std::string name) const;
		void loadTableField(std::string field) const;
		void loadGlobal(const std::string& name) const;
		void getTableField(const std::string& name, int index) const;
		//Templates
		template <typename T, typename... Args>
		int loadParams(T&& value, Args&&... args) const
		{
			pushValue(value);
			return 1 + loadParams(args...);
		}
		template <typename T>
		int loadParams(T&& value) const
		{
			pushValue(value);
			return 1;
		}
		template<typename... T>
		void checkDataValidity(unsigned int count) const
		{
			if (sizeof...(T) > count)
			{
				throw std::runtime_error("Number of template parameters passed is bigger than the number of the function's return values ");
			}
			else if (sizeof...(T) < count)
			{
				throw std::runtime_error("The number of function return values is bigger than the number of template parameters for the return values");
			}
		}
		template <typename T>
		T getValue(int stackIndex = -1) const
		{
			if (!lua_istable(m_state, -1))
			{
				std::string startText = "The variable you are trying to get is not a table, thus cannot be converted to a variable of type ";
				/*
					What we want here is just the name of the type thus we remove any references and cv qualifiers
				*/
				std::string endText = typeid(typename std::decay<T>::type).name();
				generateError(startText + endText);
			}
			std::map<std::string, std::string> data;
			auto f = [&data, this](const std::string& key) {
				if (!lua_istable(m_state, -1) && !lua_isfunction(m_state, -1))
					data[key] = getValue<std::string>(-1);
				else
					data[key] = "";
			};
			iterateTable(f);
			T result{ data };
			return result;
		}
		template <>
		double getValue<double>(int stackIndex) const
		{
			return  static_cast<double>(lua_tonumber(m_state, stackIndex));
		}
		template <>
		float getValue<float>(int stackIndex) const
		{
			return static_cast<float>(getValue<double>(stackIndex));
		}
		template <>
		int getValue<int>(int stackIndex) const
		{
			return  static_cast<int>(lua_tointeger(m_state, stackIndex));
		}
		template<>
		std::string getValue<std::string>(int stackIndex) const
		{
			size_t strLength = 0;
			const char* str = lua_tolstring(m_state, stackIndex, &strLength);
			return std::string(str, strLength);
		}
		template <>
		bool  getValue<bool>(int stackIndex) const
		{
			return lua_toboolean(m_state, stackIndex) != 0;
		}
		template <typename T>
		void pushValue(const T& val) const
		{
			createTable(val);
		}
		template <>
		void pushValue<bool>(const bool& val) const
		{
			lua_pushboolean(m_state, val);
		}
		template<>
		void pushValue<std::string>(const std::string& val) const
		{
			lua_pushlstring(m_state, val.c_str(), val.size());
		}
		template <>
		void pushValue<int>(const int& val) const
		{
			lua_pushinteger(m_state, val);
		}
		template<>
		void pushValue<double>(const double& val) const
		{
			lua_pushnumber(m_state, val);
		}
		template<>
		void pushValue<float>(const float& val) const
		{
			pushValue(static_cast<double>(val));
		}
		template <typename... Args>
		std::tuple<Args ...> getReturnValues(unsigned int functionReturnCount) const
		{
			const unsigned int popCount = functionReturnCount;
			std::tuple<Args ...> result{ getReturnValue<Args>(functionReturnCount--) ... };
			popStack(popCount);
			return result;
		}
		template <typename T>
		T getReturnValue(int index) const
		{
			if (index != 0)
			{
				return  getValue<T>(index*-1);
			}
			else
			{
				throw std::runtime_error("You cannot get return values, because there are none");
			}
		}
		template <typename T>
		void createTable(const T& val) const
		{
			lua_newtable(m_state);
			for (const auto& element : val.luaUnpack())
			{
				pushValue(element.first);
				pushValue(element.second);
				lua_settable(m_state, -3); //automatically pops [key,value] 
			}
		}
		template <typename T>
		bool setGlobal(const std::string& name, const T& val) const
		{
			try
			{
				loadGlobal(name);
				pushValue(val);
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
						pushValue<T>(val);
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
