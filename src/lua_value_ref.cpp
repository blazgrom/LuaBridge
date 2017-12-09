#include "lua_value_ref.hpp"
#include "detail/lua_error.hpp"
namespace luabz
{
const int lua_value_ref::top = -1;
const char lua_value_ref::lua_table_field_delimeter = '.';
lua_value_ref::lua_value_ref(lua_State* state, const std::string& name)
  : m_state{state}, m_name{name}, used_stack_spaces{0}
{
}

lua_value_ref::lua_value_ref(const lua_value_ref& rhs)
  : m_state{rhs.m_state}, m_name{rhs.m_name}, used_stack_spaces{0}
{
}
/**
 * \details The copy operation uses the following logic:
 * If the variables use the same lua state we simple do getglobal setglobal in
 * order to copy the values. If the values are on two different states, we take
 * the value of rhs and push it into this->m_state and then we call setglobal
 * all while retaining the type of the value from rhs. \todo Handle the case
 * when rhs element is a table
 * \todo We should transfer data from one stack to the other
 */
lua_value_ref& lua_value_ref::operator=(const lua_value_ref& rhs)
{
    rhs.load_lua_var();
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
/**
 * \todo Comment this function
 */
bool lua_value_ref::operator==(const lua_value_ref& rhs) const
{
    int lhs_index = -2, rhs_index = -1;
    load_lua_var();
    rhs.load_lua_var();
    if (m_state != rhs.m_state) {
        lua_xmove(rhs.m_state, m_state, 1);
    }
    bool result = lua_equal(m_state, lhs_index, rhs_index);

    return result;
}
/**
 * \todo Comment this function
 */
bool lua_value_ref::operator<(const lua_value_ref& rhs) const
{
    int lhs_index = -2, rhs_index = -1;
    load_lua_var();
    rhs.load_lua_var();
    if (m_state != rhs.m_state) {
        lua_xmove(rhs.m_state, m_state, 1);
    }
    bool result = lua_lessthan(m_state, lhs_index, rhs_index);

    return result;
}
bool lua_value_ref::is_nil() const
{
    load_lua_var();
    bool result = static_cast<bool>(lua_isnil(m_state, top));
    clear_used_stack_spaces();
    return result;
}
/**
 * \todo Clear up how a variable is put on the stack see lines 100->101,
 * 107->108
 */
void lua_value_ref::load_lua_var() const
{
    if (is_table_field()) {
        auto delimeter_position =
            m_name.find_first_of(lua_table_field_delimeter);
        std::string table_name = m_name.substr(0, delimeter_position);
        std::string field_name = m_name.substr(delimeter_position + 1);
        lua_getglobal(m_state, table_name.c_str());
        ++used_stack_spaces;
        while (true) {
            delimeter_position =
                field_name.find_first_of(lua_table_field_delimeter);
            bool field_reached = delimeter_position == std::string::npos;
            if (field_reached) {
                lua_getfield(m_state, top, field_name.c_str());
                ++used_stack_spaces;
                break;
            } else {
                // duplicate of line 90 -> 91
                std::string parent_field_name =
                    field_name.substr(0, delimeter_position);
                field_name = field_name.substr(delimeter_position + 1);
                lua_getfield(m_state, top, parent_field_name.c_str());
                ++used_stack_spaces;
                if (!lua_istable(m_state, top)) {
                    luabz::detail::lua_error(
                        parent_field_name +
                        " is not a table, and cannot contain the following "
                        "fields " +
                        field_name + std::to_string(used_stack_spaces) + "\n" +
                        "Original string: " + m_name + "\n");
                }
            }
        }
        return;
    }
    lua_getfield(m_state, LUA_GLOBALSINDEX, m_name.c_str());
    ++used_stack_spaces;
}
/**
 * \details Pops the top element from the state and sets the value of the
 * variable identified by m_name to the popped value
 */
void lua_value_ref::set_lua_var()
{
    lua_setglobal(m_state, m_name.c_str());
}
lua_value_ref lua_value_ref::operator[](const std::string& field_name) const
{
    load_lua_var();
    if (!lua_istable(m_state, top)) {
        luabz::detail::lua_error("What you are trying to access is not a lua "
                                 "table so it cannot be accessed as a table");
    }
    std::string full_name = m_name + lua_table_field_delimeter + field_name;
    lua_value_ref result(m_state, full_name);
    clear_used_stack_spaces();
    return result;
}
bool lua_value_ref::is_table_field() const
{
    return m_name.find(lua_table_field_delimeter) != std::string::npos;
}
/**
 * \details Pops from the stack N spaces where N is determine by the member
 * variable used_stack_spaces This function should be invoked after every
 * load_lua_variable inorder to cleanup the lua stack from values that are no
 * longer in used, if this is not done sooner or later the lua's stack would not
 * have enought space and segmentation fault would be caused
 * \todo Decide exactly where the this function should be called
 */
void lua_value_ref::clear_used_stack_spaces() const
{
    lua_pop(m_state, used_stack_spaces);
    used_stack_spaces = 0;
}
}