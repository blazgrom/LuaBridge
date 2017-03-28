#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <map>
#include <tuple>
#include <string>
#include <functional>
#include <type_traits>
#include "lua.hpp"
namespace Script
{
	struct LuaFunction
	{
		explicit LuaFunction(const std::string& name, unsigned int rC = 0)
			:
			name(name),
			resultCount(rC)
		{

		}
		std::string name;
		unsigned int resultCount;
	};
	class LuaScript
	{
	public:
		explicit LuaScript(const std::string& fileName)
			:
			_lState(luaL_newstate()),
			_fResultCount(0),
			_popC(0)
		{
			loadLuaFile(fileName);
		}
		explicit LuaScript(const char * fileName)
			:
			_lState(luaL_newstate()),
			_fResultCount(0),
			_popC(0)
		{
			loadLuaFile(fileName);
		}
		~LuaScript()
		{
			lua_close(_lState);
		}
		/*
			Gets a global variable from the file loaded during the creation of the object
		*/
		template <typename T >
		T get(const std::string& name) const
		{
			if (name.find('.') == std::string::npos)
			{
				lua_getglobal(_lState, name.c_str());
				auto val = getLuaValue<T>();
				popStack();
				return val;
			}
			else
			{
				std::string field = loadTable(name);
				loadTableField(field);
				auto result = getLuaValue<T>();
				popStack();
				return result; 
			}
		}
		/*
			Sets a global variable in the file loaded during the creation of the object
		*/
		template <typename T >
		void set(const std::string& name, const T& val) const
		{
			
			setLuaValue(val);
			lua_setglobal(_lState, name.c_str());
		}
		/*
			N input  0 output && N input N output
			When the case is  N input  0 output it creates an unnecessary tuple 
			should be fixed !
		*/
		template <typename... returnTypes, typename... Args>
		std::tuple<returnTypes...> call(const LuaFunction&function, Args... args) const
		{
			
			checkFunctionValidity<returnTypes ...>(function.resultCount);
			loadLuaFunction(function.name);
			_fResultCount = function.resultCount;
			_popC = _fResultCount;
			int numberOfArguments = loadFunctionParams(std::forward<Args>(args)...);
			callFunction(numberOfArguments, _fResultCount, "error running function " + function.name);
			return getReturnValues<returnTypes ...>();
		}
		/*
			0 input  N output
		*/
		template <typename... T>
		std::tuple<T...> call(const LuaFunction&function) const
		{
			checkFunctionValidity<T...>(function.resultCount);
			loadLuaFunction(function.name);
			_fResultCount = function.resultCount;
			_popC = _fResultCount;
			callFunction(0, _fResultCount, "error running function " + function.name);
			return getReturnValues<T...>();
		}
		/*
			 0 input 0 
		*/
		void call(const LuaFunction&function) const
		{
			if (function.resultCount > 0)
			{
				throw std::exception("Function cannot have return values");
			}
			loadLuaFunction(function.name);
			_fResultCount = 0;
			_popC = 0;
			callFunction(0, 0, "error running function " + function.name);
		}
		std::map<std::string, std::string> getInfo(const std::string& name) const
		{
			std::map<std::string, std::string> keys;
			auto process = [&keys, this]() {
				if (lua_istable(_lState, -1)) {
					auto f = [&keys, this](const std::string& key) {
						keys[key] = lua_typename(_lState, lua_type(_lState, -1));
					};
					iterateTable(f);
				}
			};
			if (name.find('.') == std::string::npos)
			{
				lua_getglobal(_lState, name.c_str());
				process();
			}
			else
			{
				std::string field= loadTable(name);
				loadTableField(field);
				process();
			}
			return keys;
		}
	private:
		lua_State* _lState;
		mutable unsigned int _fResultCount;//The result count of the last function that was called
		mutable unsigned int _popC;//Number of elements that must be popped from the stack
		/*
			Loads  a lua file + standard lib and runs it
		*/
		void loadLuaFile(const std::string&name) const
		{
			luaL_openlibs(_lState);
			if (luaL_dofile(_lState, name.c_str()))
			{
				std::string error_message = lua_tostring(_lState, -1);
				generateError(error_message);
			}
		}
		void loadLuaFunction(const std::string& name) const
		{
			auto checkType = [&name,this]() {
				if (!lua_isfunction(_lState, -1))
				{
					generateError(name + " is not a function");
				}
			};
			if (name.find('.') == std::string::npos)
			{
				lua_getglobal(_lState, name.c_str());
				checkType();
			}
			else
			{
				std::string field = loadTable(name);
				loadTableField(field);
				checkType();
			}
		
		}
		template <typename T, typename... Args>
		int loadFunctionParams(T value, Args... args) const
		{
			pushLuaValue(value);
			return 1 + loadFunctionParams(args...);
		}
		template <typename T>
		int loadFunctionParams(T value) const
		{
			pushLuaValue(value);
			return 1;
		}
		void callFunction(int numberOfArguments, int numberOfReturnValues, const std::string& errorMessage) const
		{
			if (lua_pcall(_lState, numberOfArguments, numberOfReturnValues, 0) != 0) {
				generateError(errorMessage);
			}
		}
		void generateError(const std::string& errorMessage) const
		{
			popStack();
			throw std::invalid_argument(errorMessage.c_str());
		}
		template<typename... T>
		void checkFunctionValidity(unsigned int count) const
		{
			int a = sizeof...(T);
			if (sizeof...(T) != count)
			{
				throw std::invalid_argument("You cannot get more return values than the function returns");
			}
		}
		template<typename... T>
		void checkFunctionValidity() const
		{
			checkFunctionValidity<T...>(_fResultCount);
		}
		template <typename T>
		T getLuaValue(int stackIndex = -1) const
		{
			if (lua_istable(_lState, -1))
			{
				std::map<std::string, std::string> data;
				auto f = [&data, this](const std::string& key) {
					if (!lua_istable(_lState, -1))
					{
						data[key] = getLuaValue<std::string>(-1);
					}
					else
					{
						data[key] = "";
					}
				};
				iterateTable(f);
				T result;
				result.luaPack(data);
				return result;
			}
			else
			{
				std::string startText = "The variable you are trying to get is not a table, thus cannot be converted to a variable of type ";
				std::string endText = typeid(T).name();
				generateError(startText + endText);
			}
		}
		template <>
		double getLuaValue<double>(int stackIndex) const
		{
			return  static_cast<double>(lua_tonumber(_lState, stackIndex));
		}
		template <>
		float getLuaValue<float>(int stackIndex) const
		{
			return static_cast<float>(getLuaValue<double>(stackIndex));
		}
		template <>
		int getLuaValue<int>(int stackIndex) const
		{
			return  static_cast<int>(lua_tointeger(_lState, stackIndex));
		}
		template<>
		std::string getLuaValue<std::string>(int stackIndex) const
		{
			return static_cast<std::string>(lua_tostring(_lState, stackIndex));
		}
		template <>
		bool  getLuaValue<bool>(int stackIndex) const
		{
			return lua_toboolean(_lState, stackIndex) != 0;
		}
		template <typename T>
		void setLuaValue(const T& val) const
		{
			createTable(val);
		}
		template <>
		void setLuaValue<bool>(const bool& val) const
		{
			lua_pushboolean(_lState, val);
		}
		template <>
		void setLuaValue<std::string>(const std::string& val) const
		{
			lua_pushlstring(_lState, val.c_str(), val.size());
		}
		template<>
		void setLuaValue<double>(const double& val) const
		{
			lua_pushnumber(_lState, val);
		}
		template <>
		void setLuaValue<float>(const float& val) const
		{
			setLuaValue(static_cast<double>(val));
		}
		template<>
		void setLuaValue<int>(const int& val) const
		{
			lua_pushinteger(_lState, val);
		}
		template <typename T>
		void pushLuaValue(const T& val) const
		{
			createTable(val);
		}
		template <>
		void pushLuaValue<bool>(const bool& val) const
		{
			lua_pushboolean(_lState, val);
		}
		template<>
		void pushLuaValue<std::string>(const std::string& val) const
		{
			lua_pushstring(_lState, val.c_str());
		}
		template <>
		void pushLuaValue<int>(const int& val) const
		{
			lua_pushinteger(_lState, val);
		}
		template<>
		void pushLuaValue<double>(const double& val) const
		{
			lua_pushnumber(_lState, val);
		}
		template<>
		void pushLuaValue<float>(const float& val) const
		{
			pushLuaValue(static_cast<double>(val));
		}
		void popStack(int count = 1) const
		{
			lua_pop(_lState, count);
		}
		template <typename... Args>
		std::tuple<Args ...> getReturnValues() const
		{
			checkFunctionValidity<Args...>();
			//The order of evaluation of an initializer list in c-tor is well defined!
			return tuple<Args ...>{ getReturnValue<Args>() ... };
		}
		template <typename T>
		T getReturnValue() const
		{
			if (_fResultCount != 0)
			{
				auto result = getLuaValue<T>(_fResultCount*-1);
				if ((--_fResultCount) == 0)
				{
					popStack(_popC);
					_popC = 0;
				}
				return result;
			}
			else
			{
				throw std::invalid_argument("You cannot get return values,because there are none");
			}
		}
		template <typename T>
		void createTable(const T& val) const
		{
			lua_newtable(_lState);
			for (auto element : val.luaUnpack())
			{
				pushLuaValue(element.first);
				pushLuaValue(element.second);
				lua_settable(_lState, -3);//automatically pops [key,value] 
			}
		}
		/*
			Iterates over a table and calls function process for every iteration passing the key(the value is on the stack with index = -1
			1-The function assumes that the tables is on top of the stack

		*/
		void iterateTable(std::function<void(const std::string&)> process) const
		{
			lua_pushnil(_lState);  /* first key */
			while (lua_next(_lState, -2) != 0) {
				/* uses 'key' (at index -2) and 'value' (at index -1) */
				auto key = getLuaValue<std::string>(-2);
				process(key);
				popStack();
			}
		}
		/*
			Pushes on top of the stack the first element element from name and removes it
			from name
		*/
		std::string loadTable(std::string name)const
		{
			std::string parent = name.substr(0, name.find_first_of('.'));
			lua_getglobal(_lState, parent.c_str());
			name = name.substr(name.find_first_of('.') + 1);
			return name;
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
					lua_getfield(_lState, -1, field.c_str());
					keepProcessing = false;
				}
				else
				{
					std::string parent = field.substr(0, dotPosition);
					field = field.substr(dotPosition + 1);
					lua_getfield(_lState, -1, parent.c_str());
				}
			}
		}
	};
#endif // !LUA_SCRIPT_HPP
}
