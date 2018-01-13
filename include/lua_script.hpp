#ifndef LUABZ_LUA_SCRIPT_HPP
#define LUABZ_LUA_SCRIPT_HPP
#include <cassert>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include "lua.hpp"
#include "lua_value_ref.hpp"
namespace luabz
{
/**
 * \brief The class through which the user interacts with lua
 */
class lua_script
{
  private:
    lua_State*
        m_state;  ///< The lua state on which all operations are performed
    std::string
        m_fileName;  ///< The name of the file which the lua state  represents
    bool m_open;     ///< Is the state open for operations

  public:
    lua_script();
    explicit lua_script(const std::string& file, bool lua_stl = false);
    ~lua_script() = default;
    lua_script(const lua_script& rhs) = delete;
    lua_script& operator=(const lua_script& rhs) = delete;
    lua_script(lua_script&& rhs) = default;
    lua_script& operator=(lua_script&& rhs) = default;
    /**
     * Associates the object with a lua file
     * \note This function no-op if the constructor has been invoked with
     * parameters \n if you want to change the associated lua file use change
     * \param file_name The name of the file that has to be opend
     * \param lua_stl Load lua's standard library
     */
    void open(const std::string& file_name, bool lua_stl = false);
    /**
     * \brief Closes the lua stack associated with the lua file
     * \todo Decide how to implement it
     * \todo When the stack is closed remember to clear the registered C++
     * functions
     */
    void close() noexcept;
    /**
     * \brief Changes the lua file associated with the object
     * \param file_name The name of the file that has to be opend
     * \param lua_stl Load lua's standard library
     */
    void change(const std::string& file_name, bool lua_stl = false) noexcept;
    /**
     * \brief Runs any piece of lua code on the file passed in input
     * \param lua_code The string contain lua code
     */
    void operator()(const std::string& lua_code) const;
    /**
     * \brief Access a lua global variable
     * \param name The name of the lua variable
     */
    lua_value_ref operator[](const std::string& name) const;
    /**
     * \brief Opens all standard Lua libraries into the associated lua file
     */
    void open_std() const;
};
}

/**
 * \namespace luabz
 * \brief The primary namespace of the project, contains all the classes that
 * the user has access to
 */
/**
 * \namespace luabz::detail
 * \brief A "private" namespace contains all header files that the user doesn't
 * have to see
 */
#endif