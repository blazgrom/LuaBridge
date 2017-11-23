#include "core/lua_script.hpp"
#include <algorithm>
#include "core/lua_error.hpp"
#include "core/lua_table.hpp"
namespace LuaBz
{
// Static
std::vector<lua_script::LuaCF_Intermediary> lua_script::m_registeredFunctions;
// C-tors
lua_script::lua_script() : m_stack{}, m_fileName{""}, m_open{false} {}
lua_script::lua_script(const std::string &file, bool loadStandardLib)
  : m_stack{loadStandardLib, file}, m_fileName{file}, m_open{true}
{
}
lua_script::lua_script(const std::string &file,
                       const std::vector<std::string> &dependencies,
                       bool loadStandardLib)
  : m_stack{loadStandardLib, file, dependencies}, m_fileName{file}, m_open{true}
{
}
lua_script::lua_script(const std::string &file,
                       const std::string &dependency,
                       bool loadStandardLib)
  : m_stack{loadStandardLib, file, {dependency}}, m_fileName{file}, m_open{true}
{
}

// Public
void lua_script::call(const lua_function<void> &f) const
{
    load_function(f.name());
    m_stack.call_function(0, f.result_count());
}
void lua_script::open(const std::string &file,
                      std::vector<std::string> dependencies,
                      bool loadStandardLib)
{
    if (!m_open) {
        m_stack.create(file, dependencies, loadStandardLib);
        m_open = true;
        m_fileName = file;
    }
}
void lua_script::close() noexcept
{
    m_stack.destroy();
    m_open = false;
}
bool lua_script::change(const std::string &newFile) noexcept
{
    bool result = true;
    try {
        m_stack.destroy();
        m_stack.create(newFile);
        m_fileName = newFile;
    } catch (...) {
        m_stack.destroy();
        m_stack.create(m_fileName);
        result = false;
    }
    return result;
}
void lua_script::run(std::string luaCode)
{
    m_stack.execute(luaCode);
}
void lua_script::load_function(const std::string &name) const
{
    m_stack.set_top_element(name);
    const int topElement = -1;
    if (!m_stack.is_function(topElement)) {
        throw lua_error(name + "is not a function");
    }
}
void lua_script::register_function_impl(const std::string &name)
{
    // Note:
    // The index at which the function is saved in the static C++ vector
    // becomes an upvalue for the function
    int index = m_registeredFunctions.size() - 1;
    m_stack.push(index);
    lua_CFunction lua_F = [](lua_State *functionState) -> int {
        auto functionIndex = lua_tointeger(
            functionState, lua_upvalueindex(1));  // retrieve  upvalue
        auto &function = m_registeredFunctions.at(functionIndex);
        return function(functionState);
    };
    m_stack.set_element(name, lua_F);
}
void lua_script::operator[](const std::string &name) const
{
    m_stack.set_top_element(name);
    auto variable = m_stack.top_element<int>(true);
    int a = 1;
}
}