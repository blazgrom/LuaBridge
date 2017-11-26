#ifndef LUABZ_LUA_VAL_EXPR_HPP
#define LUABZ_LUA_VAL_EXPR_HPP
#include <lua.hpp>
#include <string>
namespace LuaBz
{
class lua_script;
namespace experimental
{
class lua_value
{
    friend class LuaBz::lua_script;

  public:
    ~lua_value() = default;
    lua_value &operator=(const lua_value &rhs);
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
    lua_value &operator=(long long new_value);
    lua_value &operator=(unsigned long long new_value);
    lua_value &operator=(long new_value);
    lua_value &operator=(unsigned long new_value);
    lua_value &operator=(int new_value);
    lua_value &operator=(unsigned int new_value);
    lua_value &operator=(short new_value);
    lua_value &operator=(unsigned short new_value);
    lua_value &operator=(float new_value);
    lua_value &operator=(double new_value);
    lua_value &operator=(bool new_value);
    lua_value &operator=(char new_value);
    lua_value &operator=(const std::string &new_value);

  private:
    lua_value(lua_State *state, const std::string &name);
    lua_value(const lua_value &rhs);
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
        lua_getglobal(m_state, m_name.c_str());
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
}
#endif