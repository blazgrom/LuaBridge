#include "lua_exception.hpp"
namespace luabz
{
lua_exception::lua_exception(const std::string& msg) : std::runtime_error(msg)
{
}
lua_exception::lua_exception(const char* msg) : std::runtime_error(msg) {}
const char* lua_exception::what() const noexcept
{
    return std::runtime_error::what();
}
}  // namespace luabz
