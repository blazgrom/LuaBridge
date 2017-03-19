#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <string>
#include <type_traits>
#include "lua.hpp"
class LuaScript
{
public:
	LuaScript(const std::string&luaFile)
		:
		_lState(luaL_newstate())
	{
		luaL_openlibs(_lState);
		//Load file and execute it
		if (luaL_loadfile(_lState, luaFile.c_str())||lua_pcall(_lState,0,0,0))
		{
			std::string error_message = lua_tostring(_lState, -1);
			lua_pop(_lState, 1);
			throw std::exception(error_message.c_str());
		}
	}
	//Gets
	template <typename T >
	T get(const std::string& variableName)
	{
		lua_getglobal(_lState, variableName.c_str());
		auto value = getValue(T());
		return static_cast<T> (value);
	}

	//Sets
	~LuaScript()
	{
		lua_close(_lState);
	}

private:
	lua_State* _lState;
	lua_Number getValue(double)
	{
		return  lua_tonumber(_lState, -1);
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
};
#endif // !LUA_SCRIPT_HPP
