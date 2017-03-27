#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <functional>
#include <type_traits>
#include "lua.hpp"
/*
 *	TODO:
 *	Find better way to implement the call functionality
 */
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
		//Gets a global variable from the file loaded during the creation of the object
		template <typename T >
		T get(const std::string& variableName) const
		{
			if (variableName.find('.') == std::string::npos)
			{
				lua_getglobal(_lState, variableName.c_str());
				auto result = retrieveLuaValue<T>();
				popStack();
				return result;
			}
			else
			{
				auto lastDotPosition = variableName.find_last_of('.');
				auto table = variableName.substr(0, lastDotPosition);
				auto field = variableName.substr(lastDotPosition + 1);
				return getTableField<T>(table, field);
			}
		}
		//Sets a global variable in the file loaded during the creation of the object
		template <typename T >
		void set(const std::string& variableName, const T& value) const
		{
			setLuaValue(value);
			lua_setglobal(_lState, variableName.c_str());
		}
		//Function Calls
		template <typename... returnTypes, typename... Args>
		std::tuple<returnTypes...> call(const LuaFunction&function, Args... args) const
		{
			// N input  0 output && //N input N output
			//When the case is  N input  0 output it creates an unnecessary tuple 
			//should be fixed !
			checkParametersNumber<returnTypes ...>(function.resultCount);
			loadLuaFunction(function.name);
			_fResultCount = function.resultCount;
			_popC = _fResultCount;
			int numberOfArguments = loadFunctionParams(std::forward<Args>(args)...);
			callFunction(numberOfArguments, _fResultCount, "error running function " + function.name);
			return getResult<returnTypes ...>();
		}
		template <typename... T>
		std::tuple<T...> call(const LuaFunction&function) const
		{
			//0 input  N output
			checkParametersNumber<T...>(function.resultCount);
			loadLuaFunction(function.name);
			_fResultCount = function.resultCount;
			_popC = _fResultCount;
			callFunction(0, _fResultCount, "error running function " + function.name);
			return getResult<T...>();
		}
		void call(const LuaFunction&function) const
		{
			// 0 input 0 
			if (function.resultCount > 0)
			{
				throw std::exception("Function cannot have return values");
			}
			loadLuaFunction(function.name);
			_fResultCount = 0;
			_popC = 0;
			callFunction(0, 0, "error running function " + function.name);
		}
		std::map<std::string, std::string> getKeys(const std::string& tableName) const
		{
			std::map<std::string, std::string> keys;
			lua_getglobal(_lState, tableName.c_str());
			if (lua_istable(_lState, -1)) {
				auto f = [&keys,this](const std::string& key) {
					keys[key] = lua_typename(_lState, lua_type(_lState, -1));
				};
				retrieveTableValues(f);
			}
			return keys;
		}
	private:
		lua_State* _lState;
		mutable unsigned int _fResultCount;//The result count of the last function that was called
		mutable unsigned int _popC;//Number of elements that must be popped from the stack
		void loadLuaFile(const std::string&fileName) const
		{
			luaL_openlibs(_lState);
			//Load file and execute it
			if (luaL_dofile(_lState, fileName.c_str()))
			{
				handleError();
			}
		}
		void loadLuaFunction(const std::string& functionName) const
		{
			lua_getglobal(_lState, functionName.c_str());
			if (!lua_isfunction(_lState, -1))
			{
				generateError(functionName + " is not a function");
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
		//Call functions
		void callFunction(int numberOfArguments, int numberOfReturnValues, const std::string& errorMessage) const
		{
			if (lua_pcall(_lState, numberOfArguments, numberOfReturnValues, 0) != 0) {
				generateError(errorMessage);
			}
		}
		//Error handling
		void handleError() const
		{
			std::string error_message = lua_tostring(_lState, -1);
			generateError(error_message);
		}
		void generateError(const std::string& errorMessage) const
		{
			popStack();
			throw std::invalid_argument(errorMessage.c_str());
		}
		template<typename... T>
		void checkParametersNumber(unsigned int count) const
		{
			int a = sizeof...(T);
			if (sizeof...(T) != count)
			{
				throw std::invalid_argument("You cannot get more return values than the function returns");
			}
		}
		template<typename... T>
		void checkParametersNumber() const
		{
			checkParametersNumber<T...>(_fResultCount);
		}
		//Lua retrieve	
		template <typename T>
		T retrieveLuaValue(int stackIndex = -1) const
		{
			if (lua_istable(_lState, -1))
			{
				std::map<std::string, std::string> data;
				auto f = [&data,this](const std::string& key) {
					if (!lua_istable(_lState, -1))
					{
						data[key] = retrieveLuaValue<std::string>(-1);
					}
					else
					{
						data[key] = "";
					}
				};
				retrieveTableValues(f);
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
		double retrieveLuaValue<double>(int stackIndex) const
		{
			return  static_cast<double>(lua_tonumber(_lState, stackIndex));
		}
		template <>
		float retrieveLuaValue<float>(int stackIndex) const
		{
			return static_cast<float>(retrieveLuaValue<double>(stackIndex));
		}
		template <>
		int retrieveLuaValue<int>(int stackIndex) const
		{
			return  static_cast<int>(lua_tointeger(_lState, stackIndex));
		}
		template<>
		std::string retrieveLuaValue<std::string>(int stackIndex) const
		{
			return static_cast<std::string>(lua_tostring(_lState, stackIndex));
		}
		template <>
		bool  retrieveLuaValue<bool>(int stackIndex) const
		{
			return lua_toboolean(_lState, stackIndex) != 0;
		}
		//Sets
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
		// Push
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
		void pushLuaValue<std::string>(const std::string& str) const
		{
			lua_pushstring(_lState, str.c_str());
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
		//Utility
		void popStack(int count = 1) const
		{
			lua_pop(_lState, count);
		}
		template <typename... Args>
		std::tuple<Args ...> getResult() const
		{
			checkParametersNumber<Args...>();
			//The order of evaluation of an initializer list in c-tor is well defined!
			return tuple<Args ...>{ getResultValue<Args>() ... };
		}
		template <typename T>
		T getResultValue() const
		{
			if (_fResultCount != 0)
			{
				auto result = retrieveLuaValue<T>(_fResultCount*-1);
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
		void retrieveTableValues(std::function<void(const std::string&)> f) const
		{
			//The function assumes that the tables is on top of the stack
			lua_pushnil(_lState);  /* first key */
			while (lua_next(_lState, -2) != 0) {
				/* uses 'key' (at index -2) and 'value' (at index -1) */
				auto key = retrieveLuaValue<std::string>(-2);
				f(key);
				popStack();
			}
		}
		template<typename T>
		T getTableField(const std::string& tableName, const std::string& fieldsName) const
		{
			/*
				The function assumes that the table is a global variable, if the table is a sub table of another table this will break
			*/
			lua_getglobal(_lState, tableName.c_str());
			lua_getfield(_lState, -1, fieldsName.c_str());
			auto result = retrieveLuaValue<T>();
			popStack();
			return result;
		}
	};
#endif // !LUA_SCRIPT_HPP
}
