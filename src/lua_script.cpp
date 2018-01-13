#include "lua_script.hpp"
#include "detail/lua_error.hpp"
#include "detail/lua_state_factory.hpp"
#include "lua_exception.hpp"
#include <algorithm>
#include <utility>
namespace luabz
{
lua_script::lua_script() : m_state{nullptr}, m_fileName{""}, m_open{false} {}
lua_script::lua_script(const std::string& file, bool lua_stl)
  : m_state{detail::lua_state_factory::get_lua_state(file, lua_stl)},
    m_fileName{file},
    m_open{true}
{
}
void lua_script::open(bool load_lua_std)
{
    if (!m_open) {
        m_state =
            detail::lua_state_factory::get_lua_state(m_fileName, load_lua_std);
        m_open = true;
    }
}
void lua_script::close()
{
    //Remove registered C++ functions
    auto file_registered_functions=std::move(lua_value_ref::file_registered_functions[m_state]);
    lua_value_ref::file_registered_functions.erase(m_state);
    for(auto registered_function_pos:file_registered_functions)
    {
        lua_value_ref::registered_functions.erase(lua_value_ref::registered_functions.begin()+registered_function_pos);
    }
    m_state = nullptr;
    detail::lua_state_factory::close_lua_state(m_fileName);
    m_open = false;
    
}
void lua_script::change(const std::string& file_name, bool load_lua_std)
{
    m_state = detail::lua_state_factory::get_lua_state(file_name, load_lua_std);
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
void lua_script::open_std() const
{
    detail::lua_state_factory::open_standard_library(m_fileName);
}
}  // namespace luabz