#ifndef LUABZ_TYPES_HPP
#define LUABZ_TYPES_HPP
#include <cstddef>
#include <string>
#include "lua.hpp"
namespace LuaBz
{
enum class lua_types : uint8_t { Nil, Boolean, Integer, Number, String, Table };
struct lua_t {
    using number = float;
    using integer = int;
    using boolean = bool;
    using string = std::string;
    using nil = std::nullptr_t;
};
namespace detail
{
// Used for tag dispatching the lua_stack::get
// Represent the macro group of lua types
struct ClassType {
};
struct IntegralType {
};
struct FloatingPointType {
};
}
}
#endif