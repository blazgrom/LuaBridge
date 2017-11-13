#ifndef LUABZ_TYPES_HPP
#define LUABZ_TYPES_HPP
#include <string>
#include <cstddef>
namespace LuaBz
{
    struct type
    {
        using number=LUA_NUMBER;
        using integer=LUA_INTEGER;
        using boolean=bool;
        using string=std::string;
        using nil=std::nullptr;
    };
}
#endif