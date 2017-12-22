#include "lua_script.hpp"
#include <algorithm>
#include "detail/lua_error.hpp"
#include "detail/lua_state_factory.hpp"
#include "lua_exception.hpp"

namespace luabz
{
lua_script::lua_script() : m_state{nullptr}, m_fileName{""}, m_open{false} {}
lua_script::lua_script(const std::string& file, bool lua_stl)
  : m_state{detail::lua_state_factory::get_lua_state(file, lua_stl)},
    m_fileName{file},
    m_open{true}
{
}
void lua_script::open(const std::string& file_name, bool lua_stl)
{
    if (!m_open) {
        m_state = detail::lua_state_factory::get_lua_state(file_name, lua_stl);
        m_open = true;
        m_fileName = file_name;
    }
}
void lua_script::close() noexcept
{
    // m_stack.destroy();
    // m_open = false;
}
void lua_script::change(const std::string& file_name, bool lua_stl) noexcept
{
    m_state = detail::lua_state_factory::get_lua_state(file_name, lua_stl);
    m_open = true;
    m_fileName = file_name;
}
void lua_script::operator()(const std::string& lua_code) const
{
    static const int top = -1;
    if (luaL_dostring(m_state, lua_code.c_str())) {
        detail::lua_error(lua_tostring(m_state, top));
    }
}
lua_value_ref lua_script::operator[](const std::string& name) const
{
    return lua_value_ref{m_state, name};
}
}  // namespace luabz