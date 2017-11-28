#include "core/lua_exception.hpp"
LuaBz::lua_exception::lua_exception(const std::string &msg)
  : std::runtime_error(msg)
{
}
LuaBz::lua_exception::lua_exception(const char *msg) : std::runtime_error(msg)
{
}
const char *LuaBz::lua_exception::what() const noexcept
{
    return std::runtime_error::what();
}
