#include "core/lua_val_expr.hpp"

namespace LuaBz
{
namespace experimental
{
const int lua_value::top = -1;
lua_value::lua_value(lua_State *state, const std::string &name)
  : m_state{state}, m_name{name}
{
}

lua_value::lua_value(const lua_value &rhs)
  : m_state{rhs.m_state}, m_name{rhs.m_name}
{
}
/**
 * \details The copy operation uses the following logic:
 * If the variables use the same lua state we simple do getglobal setglobal in
 * order to copy the values. If the values are on two different states, we take
 * the value of rhs and push it into this->m_state and then we call setglobal
 * all while retaining the type of the value from rhs. \todo Handle the case
 * when rhs element is a table
 */
lua_value &lua_value::operator=(const lua_value &rhs)
{
    rhs.get_lua_var();
    if (m_state == rhs.m_state) {
        set_lua_var();
    } else {
        // TODO:
        // here we should transfer data from one stack to the other
        switch (lua_type(rhs.m_state, top)) {
            case LUA_TNIL:
                lua_pushnil(m_state);
                break;
            case LUA_TBOOLEAN:
                lua_pushboolean(m_state, lua_toboolean(rhs.m_state, top));
                break;
            case LUA_TNUMBER:
                lua_pushnumber(m_state, lua_tonumber(rhs.m_state, top));
                break;
            case LUA_TSTRING:
                lua_pushstring(m_state, lua_tostring(rhs.m_state, top));
                break;
            case LUA_TTABLE:
                break;
            case LUA_TFUNCTION:
                lua_pushcfunction(m_state, lua_tocfunction(rhs.m_state, top));
                break;
        }
        set_lua_var();
    }
    return *this;
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
    get_lua_var();
    return lua_toboolean(m_state, top);
}
lua_value::operator std::string() const
{
    get_lua_var();
    return lua_tostring(m_state, top);
}
lua_value::operator char() const
{
    get_lua_var();
    return (lua_tostring(m_state, top))[0];
}

lua_value &lua_value::operator=(long long new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(unsigned long long new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(long new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(unsigned long new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(int new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(unsigned int new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(short new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(unsigned short new_value)
{
    lua_pushinteger(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(float new_value)
{
    lua_pushnumber(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(double new_value)
{
    lua_pushnumber(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(bool new_value)
{
    lua_pushboolean(m_state, new_value);
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(char new_value)
{
    return this->operator=(std::string{new_value});
}
lua_value &lua_value::operator=(const std::string &new_value)
{
    lua_pushlstring(m_state, new_value.c_str(), new_value.size());
    set_lua_var();
    return *this;
}
lua_value &lua_value::operator=(std::nullptr_t)
{
    lua_pushnil(m_state);
    set_lua_var();
    return *this;
}

bool lua_value::is_nil() const
{
    get_lua_var();
    return static_cast<bool>(lua_isnil(m_state, top));
}
/**
 * \details Retrieves the value of the lua variable identified by m_name, thus
 * pushing it on top of the stack identified by m_state
 */
void lua_value::get_lua_var() const
{
    lua_getglobal(m_state, m_name.c_str());
}
/**
 * \details Pops the top element from the state and sets the value of the
 * variable identified by m_name to the popped value
 */
void lua_value::set_lua_var()
{
    lua_setglobal(m_state, m_name.c_str());
}
}
}