#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <string>
#include <tuple>
#include "lua.hpp"
#define DEBUG
#ifdef DEBUG
	#include <iostream>
	#include <type_traits>
#endif // DEBUG

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
		lastCalledFunctionResultCount(0),
		countToPop(0)
	{
		loadFile(fileName.c_str());
	}
	explicit LuaScript(const char * fileName)
		:
		_lState(luaL_newstate()),
		lastCalledFunctionResultCount(0),
		countToPop(0)
	{
		loadFile(fileName);
	}
	~LuaScript()
	{
		lua_close(_lState);
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
	void set(const std::string& variableName, const T& value)
	{
		setValue(value);
		lua_setglobal(_lState, variableName.c_str());
	}
	//Call function
	template <typename T, typename... Args>
	void call(const LuaFunction&function, T value, Args... args)
	{
		loadFunction(function.name);
		int numberOfArguments = loadFunctionParameters(std::forward<T>(value), std::forward<Args>(args)...);
		lastCalledFunctionResultCount = function.resultCount;
		countToPop = lastCalledFunctionResultCount;
		callFunction(numberOfArguments, lastCalledFunctionResultCount, "error running function " + function.name);
	}
	void call(const LuaFunction&function)
	{
		loadFunction(function.name);
		lastCalledFunctionResultCount = function.resultCount;
		countToPop = lastCalledFunctionResultCount;
		callFunction(0, lastCalledFunctionResultCount, "error running function " + function.name);
	}
	//Get function results
	template <typename T>
	T get_result()
	{
		if (lastCalledFunctionResultCount != 0)
		{
			auto result = getValue(T(), lastCalledFunctionResultCount*-1);
			--lastCalledFunctionResultCount;
			if (lastCalledFunctionResultCount == 0)
			{
				popStack(countToPop); 
				countToPop = 0;
			}
			return static_cast<T>(result);
		}
		else
		{
			throw std::invalid_argument("You cannot get return values,because there are none");
		}
	}
	template <typename... Args>
	std::tuple<Args ...> get_result_tuple()
	{
		std::tuple<Args ...> result;
		auto r=createTupleElement(1,"wadaw");
		/*auto tSize = std::tuple_size<std::tuple<Args ...>>::value;
		auto a= std::tuple_element_t<2, std::tuple<Args ...>>() ;*/
		//print(std::tuple_element_t<4, std::tuple<Args ...>>);
		//std::get<0>(result) = get_result<decltype(std::get<0>(result))>();
		return result;
	}
	void print(double)
	{
		std::cout << "double" << std::endl;
	}
private:
	lua_State* _lState;
	unsigned int lastCalledFunctionResultCount;
	unsigned int countToPop;
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
	template <typename T,typename... Args>
	std::tuple<T> createTupleElement(T value, Args... args)
	{
		std::tuple<T> result;
		std::get<0>(result) = get_result<T>();
		result=std::tuple_cat(result, createTupleElement(args...));
		return result;
	}
	void getTupleElemment()
	{
		return;
	}
	//Call functions
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
		popStack(1);
		throw std::invalid_argument(error_message.c_str());
	}
	void generateError(const std::string& errorMessage)
	{
		popStack(1);
		throw std::invalid_argument(errorMessage.c_str());
	}
	//Gets
	lua_Number getValue(double,int stackIndex=-1)
	{
		return  lua_tonumber(_lState, stackIndex);
	}
	lua_Number getValue(float t, int stackIndex = -1)
	{
		return getValue(static_cast<double>(t),stackIndex);
	}
	lua_Number getValue(int, int stackIndex = -1)
	{
		return  lua_tointeger(_lState, stackIndex);
	}
	std::string getValue(std::string, int stackIndex = -1)
	{
		return lua_tostring(_lState, stackIndex);
	}
	bool getValue(bool, int stackIndex = -1)
	{
		return static_cast<bool>(lua_toboolean(_lState, stackIndex));
	}
	//Sets
	void setValue(const bool val)
	{
		lua_pushboolean(_lState, val);
	}
	void setValue(const std::string& val)
	{
		lua_pushlstring(_lState, val.c_str(), val.size());
	}
	void setValue(const double val)
	{
		lua_pushnumber(_lState, val);
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
	//Utility
	void popStack( int count)
	{
		lua_pop(_lState, count);
	}
};
#endif // !LUA_SCRIPT_HPP
