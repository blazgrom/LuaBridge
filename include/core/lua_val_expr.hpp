#ifndef LUABZ_LUA_VAL_EXPR_HPP
#define LUABZ_LUA_VAL_EXPR_HPP
#include <lua.hpp>
#include <string>
namespace LuaBz
{
class lua_script;
friend class lua_script;
namespace experimental
{
class lua_value
{
  public:
    ~lua_value() = default;
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
     */
    template <typename T>
    T value_as(void (*get_value)(lua_State *state, int index))
    {
        lua_getglobal(m_state, m_name);
        return static_cast<T>(get_value(m_state, top));
    }
    lua_State *m_state;  ///< A lua state which represents the file with which
                         ///< the script was opened
    const std::string m_name;  ///< The name of the lua variable which this
                               ///< lua_value object represents
    static const int
        top;  ///< A simple identifier for the top element of the stack
};
}
}
#endif