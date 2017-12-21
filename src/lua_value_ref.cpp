#include "lua_value_ref.hpp"
#include <utility>
#include "detail/lua_error.hpp"
namespace luabz
{
const int lua_value_ref::top = -1;
const char lua_value_ref::lua_table_field_delimeter = '.';
std::vector<std::function<int(lua_State*)>> lua_value_ref::registeredFunctions;
lua_value_ref::lua_value_ref(lua_State* state, std::string name)
  : m_state{state}, m_name{std::move(name)}, used_stack_spaces{0}
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
 * all while retaining the type of the value from rhs.
 * \todo Implement this operator, see how are implemented operator < and
 * operator=
 */
lua_value_ref& lua_value_ref::operator=(const lua_value_ref& rhs)
{
    detail::lua_error("Operator= not implemented\n");

    return *this;
}
/**
 * We have two cases when confronting two lua_value_ref variables \n
 * 1) Both variable are on the same script side, in this case we just simple
 * perform load_lua_var on both variable and call lua_equal with the
 * corresponding indices 2) The variables are on two different script file, in
 * this case we first load the two variable then we perform a xmove which
 * permits us to move data from one lua stack to another lua stack, data is
 * moved from rhs into this.stack. Once we have performed the xmove we as always
 * call lua_equal \todo Refactor duplication between this function and
 * operator<()
 */
bool lua_value_ref::operator==(const lua_value_ref& rhs) const
{
    int lhs_index = -2, rhs_index = -1;
    load_lua_var();
    rhs.load_lua_var();
    if (m_state != rhs.m_state) {
        lua_xmove(rhs.m_state, m_state, 1);
        ++used_stack_spaces;
    }
    auto result = static_cast<bool>(lua_equal(m_state, lhs_index, rhs_index));
    clear_used_stack_spaces();
    rhs.clear_used_stack_spaces();
    return result;
}
/**
 * We have two cases when confronting two lua_value_ref variables \n
 * 1) Both variable are on the same script side, in this case we just simple
 * perform load_lua_var on both variable and call lua_lessthan with the
 * corresponding indices 2) The variables are on two different script file, in
 * this case we first load the two variable then we perform a xmove which
 * permits us to move data from one lua stack to another lua stack, data is
 * moved from rhs into this.stack. Once we have performed the xmove we as always
 * call lua_lessthan \todo Refactor duplication between this function and
 * operator=()
 */
bool lua_value_ref::operator<(const lua_value_ref& rhs) const
{
    int lhs_index = -2, rhs_index = -1;
    load_lua_var();
    rhs.load_lua_var();
    if (m_state != rhs.m_state) {
        lua_xmove(rhs.m_state, m_state, 1);
        ++used_stack_spaces;
    }
    auto result =
        static_cast<bool>(lua_lessthan(m_state, lhs_index, rhs_index));
    clear_used_stack_spaces();
    rhs.clear_used_stack_spaces();
    return result;
}
bool lua_value_ref::is_nil() const
{
    load_lua_var();
    auto result = static_cast<bool>(lua_isnil(m_state, top));
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
        std::string table_name =
            m_name.substr(0, delimeter_position);  /// Duplication
        std::string field_name =
            m_name.substr(delimeter_position + 1);  /// Duplication
        lua_getglobal(m_state, table_name.c_str());
        ++used_stack_spaces;
        while (true) {
            delimeter_position =
                field_name.find_first_of(lua_table_field_delimeter);
            bool field_reached = delimeter_position == std::string::npos;
            if (field_reached) {
                lua_getfield(m_state, top, field_name.c_str());
                ++used_stack_spaces;
                return;
            }
            std::string parent_field_name =
                field_name.substr(0, delimeter_position);  /// Duplication
            field_name =
                field_name.substr(delimeter_position + 1);  /// Duplication
            lua_getfield(m_state, top, parent_field_name.c_str());
            ++used_stack_spaces;
            if (!lua_istable(m_state, top)) {
                std::string error_message = parent_field_name;
                error_message += " is not a table, and cannot contain the "
                                 "following  fields ";
                error_message += field_name;
                error_message += std::to_string(used_stack_spaces);
                error_message += "\nOriginal string: ";
                error_message += m_name;
                error_message += "\n";
                detail::lua_error(error_message);
            }
        }
    }
    lua_getfield(m_state, LUA_GLOBALSINDEX, m_name.c_str());
    ++used_stack_spaces;
}
/**
 * \details Pops the top element from the state and sets the value of the
 * variable identified by m_name to the popped value
 * \todo Find a way not to use -3 here, we have to use -3 here because when we
 * are setting a table field we load the complete path of the field thus the
 * fields is at index -2 and the table is at index -3
 */
void lua_value_ref::set_lua_var()
{
    int lua_table_index = (is_table_field()) ? -3 : LUA_GLOBALSINDEX;
    std::string lua_variable_name =
        (is_table_field()) ? get_field_name() : m_name;
    lua_setfield(m_state, lua_table_index, lua_variable_name.c_str());
}
lua_value_ref lua_value_ref::operator[](const std::string& field_name) const
{
    return this->operator[](field_name.c_str());   
}
lua_value_ref lua_value_ref::operator[](const char* field_name) const
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
std::string lua_value_ref::get_field_name() const
{
    auto delimeter_position = m_name.find_last_of(lua_table_field_delimeter);
    return m_name.substr(delimeter_position + 1);
}
/**
 *  Pops from the stack N spaces where N is determine by the member
 * variable used_stack_spaces This function should be invoked after every
 * load_lua_variable inorder to cleanup the lua stack from values that are no
 * longer in used, if this is not done sooner or later the lua's stack would not
 * have enought space and segmentation fault would be caused
 * \todo Find a better way for this function to be invoke, now every time
 * load_lua_var is invoke I have to remember to invoke this at the end
 */
void lua_value_ref::clear_used_stack_spaces() const
{
    lua_pop(m_state, used_stack_spaces);
    used_stack_spaces = 0;
}
/**
 * \note The index at which the function is saved inside registredFunctions becomes
 * an upvalue of the lua_CFunction
*/
void lua_value_ref::register_function(std::size_t function_position_index)
{
    load_lua_var();
    detail::lua_value<decltype(function_position_index)>::insert(m_state,function_position_index);
    lua_CFunction new_value = [](lua_State* functionState) -> int {
        int functionIndex = lua_tointeger(
            functionState, lua_upvalueindex(1));  // retrieve  upvalue
        auto& function = registeredFunctions.at(functionIndex);
        return function(functionState);
    };
    detail::lua_value<lua_CFunction>::insert(m_state, new_value, 1);
    set_lua_var();
    clear_used_stack_spaces();
}
}  // namespace luabz