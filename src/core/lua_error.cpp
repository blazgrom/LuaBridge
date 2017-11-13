#include "core/lua_error.hpp"
using namespace LuaBz;
lua_error::lua_error(const std::string& msg)
    :
    std::runtime_error(msg)
{

}
lua_error::lua_error(const char* msg)
    :
    std::runtime_error(msg)
{

}