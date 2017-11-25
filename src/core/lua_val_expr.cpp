#include "core/lua_val_expr.hpp"
namespace LuaBz
{
namespace experimental
{
static int lua_value::top = -1;
lua_value::lua_value(lua_State *state, const std::string &name)
  : m_state{state}, m_name{name}
{
}

lua_value::lua_value(const lua_value &rhs)
  : m_state{rhs.m_state}, m_name{rhs.m_state}
{
}
lua_value::operator long long() const
{
    return value_as<long long>(lua_tointeger);
}
lua_value::operator unsigned long long() const
{
    return value_as<unsigned long long>(lua_tointeger);
}
lua_value::operator long() const
{
    return value_as<long>(lua_tointeger);
}
lua_value::operator unsigned long() const
{
    return value_as<unsigned long>(lua_tointeger);
}
lua_value::operator int() const
{
    return value_as<int>(lua_tointeger);
}
lua_value::operator unsigned int() const
{
    return value_as<unsigned int>(lua_tointeger);
}
lua_value::operator short() const
{
    return value_as<short>(lua_tointeger);
}
lua_value::operator unsigned short() const
{
    return value_as<unsigned short>(lua_tointeger);
}
lua_value::operator float() const
{
    return value_as<float>(lua_tonumber);
}
lua_value::operator double() const
{
    return value_as<double>(lua_tonumber);
}
lua_value::operator bool() const
{
    lua_getglobal(m_state, m_name);
    lua_toboolean(m_state, top);
}
lua_value::operator std::string() const
{
    lua_getglobal(m_state, m_name);
    lua_tostring(m_state, top);
}
lua_value::operator char() const
{
    lua_getglobal(m_state, m_name);
    (lua_tostring(m_state, top))[0];
}
}
}