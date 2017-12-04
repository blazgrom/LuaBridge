#include "lua_exception.hpp"
luabz::lua_exception::lua_exception(const std::string& msg)
  : std::runtime_error(msg)
{
}
luabz::lua_exception::lua_exception(const char* msg) : std::runtime_error(msg)
{
}
const char* luabz::lua_exception::what() const noexcept
{
    return std::runtime_error::what();
}
