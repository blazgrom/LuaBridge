#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <string>
#include <initializer_list>
#include "lua.hpp"
struct LuaFunctionParameter
{
	enum class DataTypes
	{
		String, Integer, FloatingPoint, Boolean
	};
	union Paramater
	{
		const char* _str;
		int  _integer;
		double _floatingpoint;
		bool _boolean;
	};
public:
	LuaFunctionParameter(const std::string& d)
		:_data(),
		_initialized(DataTypes::String)
	{
		_data._str = d.c_str();

	}
	LuaFunctionParameter(int d)
		:_data(),
		_initialized(DataTypes::Integer)
	{
		_data._integer = d;

	}
	LuaFunctionParameter(double d)
		:_data(),
		_initialized(DataTypes::FloatingPoint)

	{
		_data._floatingpoint = d;
	}
	LuaFunctionParameter(bool d)
		:_data(),
		_initialized(DataTypes::Integer)
	{
		_data._boolean = d;
	}
	DataTypes getInitializedType()const
	{
		return _initialized;
	}
	int getInteger() const
	{
		return _data._integer;
	}
	double getFloatingPoint() const
	{
		return _data._floatingpoint;
	}
	const char* getString() const
	{
		return _data._str;
	}
	bool getBoolean() const
	{
		return _data._boolean;
	}
private:
	Paramater _data;
	DataTypes _initialized;
};
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
	//Gets a global variable from the file loaded during the creation of the object
	template <typename T >
	T getAs(const std::string& variableName)
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
	void execute(const std::string& functionName, const std::initializer_list<LuaFunctionParameter> params,const std::string& resultVariableName)
	{
		lua_getfield(_lState, LUA_GLOBALSINDEX, functionName.c_str());
		for (const LuaFunctionParameter& param : params)
		{
			auto type = param.getInitializedType();
			switch (type)
			{
			case LuaFunctionParameter::DataTypes::String:
				pushValue(param.getString());
				break;
			case LuaFunctionParameter::DataTypes::Integer:
				pushValue(param.getInteger());
				break;
			case LuaFunctionParameter::DataTypes::FloatingPoint:
				pushValue(param.getFloatingPoint());
				break;
			case LuaFunctionParameter::DataTypes::Boolean:
				pushValue(param.getBoolean());
				break;
			default:
				break;
			}
		}
		lua_call(_lState, params.size(), 1);
		lua_setfield(_lState, LUA_GLOBALSINDEX, resultVariableName.c_str());
	}
	~LuaScript()
	{
		lua_close(_lState);
	}

private:
	lua_State* _lState;
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
