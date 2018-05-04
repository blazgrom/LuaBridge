#pragma once
#include "error.hpp"
#include "state.hpp"
#include "luabz_exception.hpp"
#include "var_ref.hpp"
#include <algorithm>
#include <lua.hpp>
#include <string>
#include <utility>

namespace luabz
{
/**
 * \brief The class through which the user interacts with lua
 */
class script
{
  private:
    std::string m_fileName;
    lua_State* m_state;
    bool m_isStateActive;

  public:
    script() : m_fileName{""}, m_state{nullptr}, m_isStateActive{false} {}

    explicit script(const std::string& file, bool load_lua_std = false)
      : m_fileName{file},
        m_state{state::get(file, load_lua_std)},
        m_isStateActive{true}
    {
    }

    ~script(){
        // TODO: Decide if this should close itself, and if this would impact the other code
        // if(m_isStateActive){
        //     close();
        // }
    };

    
    script(const script& rhs) = delete;

    script& operator=(const script& rhs) = delete;

    script(script&& rhs) = default;

    script& operator=(script&& rhs) = default;

    /**
     * Associates the object with a lua file
     * \note This function no-op if the constructor has been invoked with
     * parameters \n if you want to change the associated lua file use change
     * \param lua_stl Load lua's standard library
     */
    void open(bool load_lua_std = false)
    {
        if (!m_isStateActive) {
            m_state = state::get(m_fileName, load_lua_std);
            m_isStateActive = true;
        }
    }

    /**
     * \brief Closes the lua stack associated with the lua file
     */
    void close() noexcept
    {
        state::close(m_fileName);
        // var_ref::remove_registed_cpp_functions(m_state);
        m_state = nullptr;
        m_isStateActive = false;
    }

    /**
     * \brief Changes the lua file associated with the object
     * \param file_name The name of the file that has to be opend
     * \param lua_stl Load lua's standard library
     */
    void change(const std::string& file_name, bool load_lua_std = false)
    {
        if (m_isStateActive) {
            close();
        }
        m_state = state::get(file_name, load_lua_std);
        m_isStateActive = true;
        m_fileName = file_name;
    }

    void operator()(const std::string& lua_code) const
    {
        if (luaL_dostring(m_state, lua_code.c_str())) {
            error(lua_tostring(m_state, -1));
        }
    }

    var_ref operator[](const std::string& name) const { return var_ref{m_state, name}; }

    void open_std() const { state::open_std(m_fileName); }
};
}  // namespace luabz