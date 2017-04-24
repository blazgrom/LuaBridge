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
		explicit LuaScript(const std::string& file)
			:
			m_state(luaL_newstate()),
			m_fileName(file),
			m_open(true)
		{
			luaL_openlibs(m_state);
			loadFile(m_fileName);
		}
		LuaScript(const std::string& file,const std::vector<std::string>& dependencies)
			:
			m_state(luaL_newstate()),
			m_fileName(file),
			m_open(true)
		{
			initialize( dependencies);
		}
		LuaScript(const std::string& file, const std::string& dependency)
			:
			m_state(luaL_newstate()),
			m_fileName(file),
			m_open(true)
		{
			initialize({ dependency });
		}
		~LuaScript()
		{
			lua_close(m_state);
		}
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
		bool set(const std::string& name,  T&& val) const
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
			callFunction(argumentsCount, f.resultCount,  f.name);
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
			callFunction(0, function.resultCount,  function.name);
			return getReturnValues<T...>(function.resultCount);
		}
		void call(const LuaFunction<void>& function) const
		{
			loadFunction(function.name);
			callFunction(0, function.resultCount,  function.name);
		}
		std::map<std::string, std::string> getTableInfo(const std::string& name) const
		{
			std::map<std::string, std::string> info;
			auto process = [&info, this]() {
				if (lua_istable(m_state, -1)) {
					auto f = [&info, this](const std::string& key) {
						info[key] = lua_typename(m_state, lua_type(m_state, -1));
					};
					iterateTable(f);
				}
			};
			if (name.find('.') == std::string::npos)
			{
				loadGlobal(name);
			}
			else
			{
				std::string field= loadTable(name);
				loadTableField(field);
			}
			process();
			return info;
		}
		void openFile(const std::string& name)
		{
			if (!m_open)
			{
				m_state = luaL_newstate();
				loadFile(name);
				m_open = true;
				m_fileName = name;
			}
		}
		void closeFile() noexcept
		{
			lua_close(m_state);
			m_open = false;
		}
		bool changeFile(const std::string& name) noexcept
		{
			bool result = true;
			lua_close(m_state);
			m_state = luaL_newstate();
			try
			{
				loadFile(name);
				m_fileName = name;
			}
			catch (...)
			{
				loadFile(m_fileName);
				result = false;
			}
			return result;
		}
	private:
		void initialize(const std::vector<std::string>& dependencies)
		{
			luaL_openlibs(m_state);
			for (const std::string& file : dependencies)
			{
				loadFile(file);
			}
			loadFile(m_fileName);
		}
		/*
			Loads  a lua file + standard lib and runs it
		*/
		void loadFile(const std::string& name) const
		{
			if (luaL_dofile(m_state, name.c_str()))
			{
				generateError(lua_tostring(m_state, -1));
			}
		}
		void loadFunction(const std::string& name) const
		{
			auto checkType = [&name,this]() {
				if (!lua_isfunction(m_state, -1))
				{
					popStack();
					throw std::runtime_error(name + " is not a function");
				}
			};
			if (name.find('.') == std::string::npos)
			{
				loadGlobal(name);
			}
			else
			{
				std::string field = loadTable(name);
				loadTableField(field);
			}
			checkType();
		}
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
		void callFunction(int inputValCount, int outputValCount, const std::string& functionName) const
		{
			if (lua_pcall(m_state, inputValCount, outputValCount, 0) != 0) {
				generateError(functionName+" : " + getValue<std::string>());
			}
		}
		void generateError(const std::string& message) const
		{
			popStack();
			throw std::exception(message.c_str());
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
			T result{data};
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
		void popStack(int count = 1) const
		{
			lua_pop(m_state, count);
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
		/*
			Iterates over a table and calls function process for every iteration passing the key(the value is on the stack with index = -1
			1-The function assumes that the tables is on top of the stack
		*/
		void iterateTable(std::function<void(const std::string&)> process) const
		{
			lua_pushnil(m_state);  
			while (lua_next(m_state, -2) != 0)
			{
				/*key=-2, value=-1*/
				auto key = getValue<std::string>(-2);
				process(key);
				popStack();
			}
		}
		/*
			Pushes on top of the stack the first element  from name and return the names of the remaining fields
		*/
		std::string loadTable(std::string name) const
		{
			std::string parent = name.substr(0, name.find_first_of('.'));
			loadGlobal(parent);
			return name.substr(name.find_first_of('.') + 1);
		}
		/*
			Pushes on top of the stack the last element from the str, where each element is 
			separed with .(dot)
			1-The function assumes that the parent table is already on top of the stack
		*/
		void loadTableField(std::string field) const
		{
			
			auto keepProcessing = true;
			while (keepProcessing)
			{
				auto dotPosition = field.find_first_of('.');
				if (dotPosition == std::string::npos)
				{
					keepProcessing = false;
					getTableField(field, -1);
				}
				else
				{
					std::string parent = field.substr(0, dotPosition);
					field = field.substr(dotPosition + 1);
					getTableField(parent, -1);
				}
			}
		}
		void loadGlobal(const std::string& name) const
		{
			lua_getglobal(m_state, name.c_str());
			bool failed = lua_isnoneornil(m_state, -1);
			if (failed)
			{
				popStack();
				throw std::runtime_error("Object with name " + name + " could not be loaded");
			}
		}
		/*
			Pushes on top of the stack the field with name equals to name of the table at specified index,
			if the table doesnt contain a specific field an exception is thrown 
		*/
		void getTableField(const std::string& name, int index) const
		{
			if (lua_istable(m_state, index))
			{
				lua_getfield(m_state, index, name.c_str());
			}
			else
			{
				popStack();
				throw std::runtime_error("The field " + name + " could not be loaded");
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
