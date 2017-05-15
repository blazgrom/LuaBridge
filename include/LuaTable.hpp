#ifndef LUA_HELPERS_HPP
#define LUA_HELPERS_HPP
#include <string>
#include <vector>
#include "LuaValue.hpp"
namespace Lua
{
	struct LuaTable
	{
		std::vector<LuaValue> values;
	};
}
#endif // !LUA_HELPERS_HPP
