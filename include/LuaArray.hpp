#ifndef LUABZ_LUA_ARRAY_HPP
#define LUABZ_LUA_ARRAY_HPP
#include <LuaContainer.hpp>
namespace LuaBz
{
	//LuaArray is a Homogeneous LuaContainer that does not contains holes,
	//this type is different from the one defined in Lua as array
	//Lua definition: table that has as key integers values
	template<class T>
	using LuaArray = LuaContainer<T>;
}
#endif // !LUABZ_LUA_ARRAY_HPP
