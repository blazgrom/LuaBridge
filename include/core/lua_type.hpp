#ifndef LUABZ_TYPES_HPP
#define LUABZ_TYPES_HPP
#include <string>
#include "lua.hpp"
#include <cstddef>
namespace LuaBz
{
    enum class lua_types : uint8_t
	{
		Nil, Boolean, Integer, Number, String, Table
	};
    struct lua_t
    {
        using number=LUA_NUMBER;
        using integer=LUA_INTEGER;
        using boolean=bool;
        using string=std::string;
        using nil=std::nullptr_t;
    };
}
#endif