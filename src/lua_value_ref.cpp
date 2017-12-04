#include "lua_value_ref.hpp"

namespace luabz
{
const int lua_value_ref::top = -1;
lua_value_ref::lua_value_ref(lua_State *state, const std::string &name)
  : m_state{state}, m_name{name}
{
}

lua_value_ref::lua_value_ref(const lua_value_ref &rhs)
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
lua_value_ref &lua_value_ref::operator=(const lua_value_ref &rhs)
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

bool lua_value_ref::is_nil() const
{
    get_lua_var();
    return static_cast<bool>(lua_isnil(m_state, top));
}
/**
 * \details Retrieves the value of the lua variable identified by m_name, thus
 * pushing it on top of the stack identified by m_state
 */
void lua_value_ref::get_lua_var() const
{
    lua_getglobal(m_state, m_name.c_str());
}
/**
 * \details Pops the top element from the state and sets the value of the
 * variable identified by m_name to the popped value
 */
void lua_value_ref::set_lua_var()
{
    lua_setglobal(m_state, m_name.c_str());
}
}