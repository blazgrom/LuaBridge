#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <string>
#include <initializer_list>
#include "lua.hpp"
class LuaScript
{
public:
	
	explicit LuaScript(const std::string& fileName)
		:
		_lState(luaL_newstate())
	{
		loadFile(fileName.c_str());
	}
	explicit LuaScript(const char * fileName)
		:
		_lState(luaL_newstate())
	{
		loadFile(fileName);
	}
	//Gets a global variable from the file loaded during the creation of the object
	template <typename T >
	T get(const std::string& variableName)
	{
		lua_getglobal(_lState, variableName.c_str());
			auto value = getValue(T());
			return static_cast<T> (value);
	}
	//Sets a global variable in the file loaded during the creation of the object
	template <typename T >
	void set(const std::string& variableName,const T& value)
	{
		setValue(value);
		lua_setglobal(_lState, variableName.c_str());
	}
	//Call function
	template <typename T, typename... Args>
	void call(const std::string& functionName, T value, Args... args)
	{
		loadFunction(functionName);
		int numberOfArguments = loadFunctionParameters(std::forward<T>(value), std::forward<Args>(args)...);
		int numberOfReturnValues = 0;
		callFunction(numberOfArguments, numberOfReturnValues, "error running function " + functionName);
	}
	~LuaScript()
	{
		lua_close(_lState);
	}

private:
	lua_State* _lState;
	//Load functions
	void loadFile(const char *fileName)
	{
		luaL_openlibs(_lState);
		//Load file and execute it
		if (luaL_dofile(_lState, fileName))
		{
			handleError();
		}
	}
	void loadFunction(const std::string& functionName)
	{
		lua_getglobal(_lState, functionName.c_str());
		if (!lua_isfunction(_lState, -1))
		{
			generateError(functionName + " is not a function");
		}
	}
	template <typename T, typename... Args>
	int loadFunctionParameters(T value, Args... args)
	{
		pushValue(value);
		return 1 + loadFunctionParameters(args...);
	}
	int loadFunctionParameters()
	{
		//Function used only to break the variadric recursion
		return 0;
	}
	void callFunction(int numberOfArguments, int numberOfReturnValues, const std::string& errorMessage)
	{
		if (lua_pcall(_lState, numberOfArguments, numberOfReturnValues, 0) != 0) {
			generateError(errorMessage);
		}
	}
	//Error handling
	void handleError()
	{
		std::string error_message = lua_tostring(_lState, -1);
		lua_pop(_lState, 1);
		throw std::invalid_argument(error_message.c_str());
	}
	void generateError(const std::string& errorMessage)
	{
		lua_pop(_lState, 1);
		throw std::invalid_argument(errorMessage.c_str());
	}
	//Gets
	lua_Number getValue(double)
	{
		return  lua_tonumber(_lState, -1);
	}
	lua_Number getValue(float t)
	{
		return getValue(static_cast<double>(t));
	}
	lua_Number getValue(int)
	{
		return  lua_tointeger(_lState, -1);
	}
	const char* getValue(std::string)
	{
		return lua_tostring(_lState, -1);
	}
	bool getValue(bool)
	{
		return lua_toboolean(_lState, -1);
	}
	//Sets
	void setValue(const bool val)
	{
		 lua_pushboolean(_lState, val);
	}
	void setValue(const std::string& val)
	{
		lua_pushlstring(_lState, val.c_str(),val.size());
	}
	void setValue(const double val)
	{
		lua_pushnumber(_lState,val);
	}
	void setValue(const float val)
	{
		setValue(static_cast<double>(val));
	}
	void setValue(const int val)
	{
		lua_pushinteger(_lState, val);
	}
	// Push
	void pushValue(bool val)
	{
		lua_pushboolean(_lState, val);
	}
	void pushValue(const char* str)
	{
		lua_pushstring(_lState, str);
	}
	void pushValue(int val)
	{
		lua_pushinteger(_lState, val);
	}
	void pushValue(double val)
	{
		lua_pushnumber(_lState, val);
	}
	void pushValue(float val)
	{
		pushValue(static_cast<double>(val));
	}
};
#endif // !LUA_SCRIPT_HPP
