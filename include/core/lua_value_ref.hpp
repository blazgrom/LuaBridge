#ifndef luabz_LUA_VAL_EXPR_HPP
#define luabz_LUA_VAL_EXPR_HPP
#include <cstddef>
#include <lua.hpp>
#include <string>
namespace luabz
{
class lua_script;

class lua_value_ref
{
    friend class luabz::lua_script;

  public:
    ~lua_value_ref() = default;
    lua_value_ref &operator=(const lua_value_ref &rhs);
    operator long long() const;
    operator unsigned long long() const;
    operator long() const;
    operator unsigned long() const;
    operator int() const;
    operator unsigned int() const;
    operator short() const;
    operator unsigned short() const;
    operator float() const;
    operator double() const;
    operator bool() const;
    operator char() const;
    operator std::string() const;
    lua_value_ref &operator=(long long new_value);
    lua_value_ref &operator=(unsigned long long new_value);
    lua_value_ref &operator=(long new_value);
    lua_value_ref &operator=(unsigned long new_value);
    lua_value_ref &operator=(int new_value);
    lua_value_ref &operator=(unsigned int new_value);
    lua_value_ref &operator=(short new_value);
    lua_value_ref &operator=(unsigned short new_value);
    lua_value_ref &operator=(float new_value);
    lua_value_ref &operator=(double new_value);
    lua_value_ref &operator=(bool new_value);
    lua_value_ref &operator=(char new_value);
    lua_value_ref &operator=(const std::string &new_value);
    lua_value_ref &operator=(std::nullptr_t);
    /**
     * Checks if the value identified by the name of lua_value_ref is equal to lua's
     * nil
     */
    bool is_nil() const;

  private:
    lua_value_ref(lua_State *state, const std::string &name);
    lua_value_ref(const lua_value_ref &rhs);
    /**
     * \brief Retrieves value from the stack
     *
     * Retrieves value from the stack identified by m_state using the function
     * ptr passed as input parameter. After the value is retrived the element is
     * converted to type T which is passed as template parameter \param
     * get_value The function that will be used to retrieve the value from the
     * stack \returns The lua variables identified by m_name transformed into
     * type T
     * \todo Decide if it makes sense to be const
     */
    template <typename T, typename OriginalType>
    T value_as(OriginalType (*get_value)(lua_State *state, int index)) const
    {
        get_lua_var();
        return static_cast<T>(get_value(m_state, top));
    }
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
    lua_State *m_state;  ///< A lua state which represents the file with which
                         ///< the script was opened
    const std::string
        m_name;            ///< The name of the lua variable which the object
                           ///< represents
    static const int top;  ///< A simple identifier for the top element of the
                           ///< stack
};
}
#endif