#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <string>
#include <tuple>
#include "lua.hpp"
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
		_fResultCount(0),
		_popC(0)
	{
		load_lua_file(fileName.c_str());
	}
	explicit LuaScript(const char * fileName)
		:
		_lState(luaL_newstate()),
		_fResultCount(0),
		_popC(0)
	{
		load_lua_file(fileName);
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
		auto value = retrieve_lua_value<T>();
		return value;
	}
	//Sets a global variable in the file loaded during the creation of the object
	template <typename T >
	void set(const std::string& variableName, const T& value)
	{
		set_lua_value(value);
		lua_setglobal(_lState, variableName.c_str());
	}
	//Call function
	/*
	*	TODO:
	*	Find better way to implement the call functionality
	*/
	template <typename T, typename... Args>
	void call(const LuaFunction&function, T value, Args... args)
	{
		load_lua_function(function.name);
		int numberOfArguments = luad_function_params(std::forward<T>(value), std::forward<Args>(args)...);
		_fResultCount = function.resultCount;
		_popC = _fResultCount;
		call_function(numberOfArguments, _fResultCount, "error running function " + function.name);
	}
	void call(const LuaFunction&function)
	{
		load_lua_function(function.name);
		_fResultCount = function.resultCount;
		_popC = _fResultCount;
		call_function(0, _fResultCount, "error running function " + function.name);
	}
	//Get function results
	template <typename T>
	T get_r()
	{
		if (_fResultCount != 0)
		{
			auto result = retrieve_lua_value<T>(_fResultCount*-1);
			--_fResultCount;
			if (_fResultCount == 0)
			{
				pop_stack(_popC); 
				_popC = 0;
			}
		}
		else
		{
			throw std::invalid_argument("You cannot get return values,because there are none");
		}
	}
	template <typename... Args>
	std::tuple<Args ...> get_r_tuple()
	{
		if (check_tuple_size<Args ...>() != _fResultCount)
		{
			throw std::invalid_argument("You cannot get more return values than the function returns");
		}
		auto result= std::make_tuple(get_r<Args>()...);
		popStack(_popC);
		_popC = 0;
		return result;
	}
private:
	lua_State* _lState;
	unsigned int _fResultCount;//The result count of the last function that was called
	unsigned int _popC;//Number of elements that must be popped from the stack
	//Load functions
	void load_lua_file(const char *fileName)
	{
		luaL_openlibs(_lState);
		//Load file and execute it
		if (luaL_dofile(_lState, fileName))
		{
			handle_error();
		}
	}
	void load_lua_function(const std::string& functionName)
	{
		lua_getglobal(_lState, functionName.c_str());
		if (!lua_isfunction(_lState, -1))
		{
			generate_error(functionName + " is not a function");
		}
	}
	template <typename T, typename... Args>
	int luad_function_params(T value, Args... args)
	{
		push(value);
		return 1 + luad_function_params(args...);
	}
	int luad_function_params()
	{
		//Function used only to break the variadric recursion
		return 0;
	}
	template <typename... Args>
	int check_tuple_size()
	{
		return std::tuple_size<std::tuple<Args ...>>::value;
	}
	//Call functions
	void call_function(int numberOfArguments, int numberOfReturnValues, const std::string& errorMessage)
	{
		if (lua_pcall(_lState, numberOfArguments, numberOfReturnValues, 0) != 0) {
			generate_error(errorMessage);
		}
	}
	//Error handling
	void handle_error()
	{
		std::string error_message = lua_tostring(_lState, -1);
		pop_stack();
		throw std::invalid_argument(error_message.c_str());
	}
	void generate_error(const std::string& errorMessage)
	{
		pop_stack();
		throw std::invalid_argument(errorMessage.c_str());
	}
	//Lua retrieve	
	template <typename T>
	T retrieve_lua_value(int stackIndex =-1 )
	{
		/*
		 *	TODO:
		 *  Add user data to the list of luaintegration handled types
		 *
		 */
		return T;
	}
	template <>
	lua_Number retrieve_lua_value<double>(int stackIndex)
	{
		return  static_cast<double>(lua_tonumber(_lState, stackIndex));
	}
	template <>
	float retrieve_lua_value<float>(int stackIndex)
	{
		return retrieve_lua_value<double>(stackIndex);
	}
	template <>
	int retrieve_lua_value<int>(int stackIndex)
	{
		return  static_cast<int>(lua_tointeger(_lState, stackIndex));
	}
	template<>
	std::string retrieve_lua_value<std::string>( int stackIndex)
	{
		return static_cast<std::string>(lua_tostring(_lState, stackIndex));
	}
	template <>
	bool  retrieve_lua_value<bool>(int stackIndex )
	{
		return lua_toboolean(_lState, stackIndex) !=0;
	}
	//Sets
	template <typename T>
	void set_lua_value(const T& val)
	{
		/*
		*	TODO:
		*  Add user data to the list of luaintegration handled types
		*
		*/
	}
	template <>
	void set_lua_value<bool>(const bool& val)
	{
		lua_pushboolean(_lState, val);
	}
	template <>
	void set_lua_value<std::string>(const std::string& val)
	{
		lua_pushlstring(_lState, val.c_str(), val.size());
	}
	template<>
	void set_lua_value<double>(const double& val)
	{
		lua_pushnumber(_lState, val);
	}
	template <>
	void set_lua_value<float>(const float& val)
	{
		set_lua_value(static_cast<double>(val));
	}
	template<>
	void set_lua_value<int>(const int& val)
	{
		lua_pushinteger(_lState, val);
	}
	// Push
	template <typename T>
	void push (T val)
	{
		/*
		*	TODO:
		*  Add user data to the list of luaintegration handled types
		*
		*/
	}
	template <>
	void push<bool>(bool val)
	{
		lua_pushboolean(_lState, val);
	}
	template<>
	void push<std::string>( std::string str)
	{
		lua_pushstring(_lState, str.c_str());
	}
	template <>
	void push<int>(int val)
	{
		lua_pushinteger(_lState, val);
	}
	template<>
	void push<double>(double val)
	{
		lua_pushnumber(_lState, val);
	}
	template<>
	void push<float>(float val)
	{
		push(static_cast<double>(val));
	}
	//Utility
	void pop_stack( int count=1)
	{
		lua_pop(_lState, count);
	}
};
#endif // !LUA_SCRIPT_HPP
