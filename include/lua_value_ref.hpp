#ifndef luabz_LUA_VAL_EXPR_HPP
#define luabz_LUA_VAL_EXPR_HPP
#include <cstddef>
#include <lua.hpp>
#include <string>
#include "detail/lua_value.hpp"
namespace luabz
{
class lua_script;

class lua_value_ref
{
    friend class luabz::lua_script;

  public:
    ~lua_value_ref() = default;
    lua_value_ref& operator=(const lua_value_ref& rhs);
    template <typename T>
    operator T() const
    {
        return detail::lua_value<T>::get(m_state, m_name);
    }
    template <typename T>
    lua_value_ref& operator=(const T& new_value)
    {
        detail::lua_value<T>::set(m_state, m_name, new_value);
        return *this;
    }
    template <typename T>
    bool operator==(const T& rhs)
    {
        T lhs = *this;  // Implicit cast to type T
        return lhs == rhs;
    }
    bool operator==(const lua_value_ref& rhs);
    template <typename T>
    bool operator<(const T& rhs)
    {
        T lhs = *this;  // Implicit cast to type T
        return lhs < rhs;
    }
    bool operator<(const lua_value_ref& rhs);
    /**
     * Checks if the value identified by the name of lua_value_ref is equal to
     * lua's nil
     */
    bool is_nil() const;

  private:
    lua_value_ref(lua_State* state, const std::string& name);
    lua_value_ref(const lua_value_ref& rhs);
    /**
     * \brief Retrieves the value of the lua variable
     */
    void get_lua_var() const;
    /**
     * \brief Sets the lua variable to a new value
     * \pre The new value is already be on top of the lua stack
     * (m_state in this case).
     */
    void set_lua_var();
    lua_State* m_state;  ///< A lua state which represents the file with which
                         ///< the script was opened
    const std::string
        m_name;            ///< The name of the lua variable which the object
                           ///< represents
    static const int top;  ///< A simple identifier for the top element of the
                           ///< stack
};
}
#endif