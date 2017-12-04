#include "core/lua_script.hpp"
#include <algorithm>
#include "core/lua_exception.hpp"
#include "detail/lua_state_factory.hpp"
namespace LuaBz
{
// C-tors
// TODO :
// Implement all empty functions
lua_script::lua_script()  :
m_state{nullptr},
m_fileName{""},
m_open{false}
{
}
lua_script::lua_script(const std::string &file, bool loadStandardLib) : m_state{detail::lua_state_factory::create_state(file,loadStandardLib)},
m_fileName{file},
m_open{true} {}
lua_script::lua_script(const std::string &file,
                       const std::vector<std::string> &dependencies,
                       bool loadStandardLib)
                       : m_state{detail::lua_state_factory::create_state(file,loadStandardLib)},
m_fileName{file},
m_open{true} 

{
}
lua_script::lua_script(const std::string &file,
                       const std::string &dependency,
                       bool loadStandardLib)
                       : m_state{detail::lua_state_factory::create_state(file,loadStandardLib)},
m_fileName{file},
m_open{true} 

{
}

void lua_script::open(const std::string &file,
                      std::vector<std::string> dependencies,
                      bool loadStandardLib)
{
    if (!m_open) {
        m_state=detail::lua_state_factory::create_state(file,loadStandardLib);
        m_open = true;
        m_fileName = file;
    }
}
void lua_script::close() noexcept
{
    // m_stack.destroy();
    // m_open = false;
}
bool lua_script::change(const std::string &newFile) noexcept
{
    bool result = true;
    // try {
    //     m_stack.destroy();
    //     m_stack.create(newFile);
    //     m_fileName = newFile;
    // } catch (...) {
    //     m_stack.destroy();
    //     m_stack.create(m_fileName);
    //     result = false;
    // }
    return result;
}
/**
 * \todo topElement should not be defined here \n
 * Fine a better way  to communicate that a lua error has occured
 */
void lua_script::operator()(const std::string &lua_code) const
{
    static const int top = -1;
    if (luaL_dostring(m_state, lua_code.c_str())) {
        throw lua_exception(lua_tostring(m_state, top));
    }
}
experimental::lua_value lua_script::operator[](const std::string &name) const
{
    return experimental::lua_value{m_state, name};
}
}