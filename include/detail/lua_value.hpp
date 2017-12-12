#ifndef LUABZ_LUA_VALUE_HPP
#define LUABZ_LUA_VALUE_HPP
#include <lua.hpp>
#include <string>
namespace luabz
{
namespace detail
{
/**
 * \brief The main class used for retrieving/inserting values from/into a lua
 * stack. \pre Every specialization has to provide a static member function
 * called "insert" which accepts \n lua_State* and addional parameter which can
 * be anything. \pre Every specialization has to provice a static member
 * function called "get" accepting lua_State and which convert and returns the
 * \n top element of the stack to the desired type. \note If one of the required
 * function is not provided you will not be the corresponding functionality with
 * the new type.
 */
template <class T>
struct lua_value;

/**
 * \brief Specialization for const bool
 */
template <>
struct lua_value<const bool> {
    /**
     *  Contains all the logic of how a C++ const bool is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, const bool& value)
    {
        lua_pushboolean(state, value);
    }
    /**
     *  Contains all the logic of how a Lua boolean is converted tp C++ boolean
     */
    static bool get(lua_State* state)
    {
        return static_cast<bool>(lua_toboolean(state, -1));
    }
};
/**
 * \brief Specialization for bool
 */
template <>
struct lua_value<bool> {
    /**
     * Contains all the logic of how a C++  bool is inserted in the lua stack
     */
    static void insert(lua_State* state, bool value)
    {
        lua_pushboolean(state, value);
    }
    /**
     * Contains all the logic of how a Lua boolean is converted to C++ boolean
     */
    static bool get(lua_State* state)
    {
        return static_cast<bool>(lua_toboolean(state, -1));
    }
};
/**
 * \brief Specialization for const long long
 */
template <>
struct lua_value<const long long> {
    /**
     *  Contains all the logic of how a C++  const long long is inserted in the
     * lua stack
     */
    static void insert(lua_State* state, const long long& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++ long long
     */
    static long long get(lua_State* state)
    {
        return static_cast<long long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for long long
 */
template <>
struct lua_value<long long> {
    /**
     *  Contains all the logic of how a C++   long long is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, long long value)
    {
        lua_pushinteger(state, value);
    }
    /**
     *  Contains all the logic of how a Lua number is converted to  C++ long
     * long
     */
    static long long get(lua_State* state)
    {
        return static_cast<long long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for const unsigned long long
 */
template <>
struct lua_value<const unsigned long long> {
    /**
     *  Contains all the logic of how a C++  const unsigned long long is
     * inserted in the lua stack
     */
    static void insert(lua_State* state, const unsigned long long& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     *  Contains all the logic of how a Lua number is converted to  C++ unsigned
     * long long
     */
    static unsigned long long get(lua_State* state)
    {
        return static_cast<unsigned long long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for unsigned long long
 */
template <>
struct lua_value<unsigned long long> {
    /**
     *  Contains all the logic of how a C++ unsigned long long is inserted in
     * the lua stack
     */
    static void insert(lua_State* state, unsigned long long value)
    {
        lua_pushinteger(state, value);
    }
    /**
     *  Contains all the logic of how a Lua number is converted to  C++ unsigned
     * long long
     */
    static unsigned long long get(lua_State* state)
    {
        return static_cast<unsigned long long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for const  long
 */
template <>
struct lua_value<const long> {
    /**
     *  Contains all the logic of how a C++ const  long is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, const long& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     *  Contains all the logic of how a Lua number is converted to  C++   long
     */
    static long get(lua_State* state)
    {
        return static_cast<long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for  long
 */
template <>
struct lua_value<long> {
    /**
     *  Contains all the logic of how a C++   long is inserted in the lua stack
     */
    static void insert(lua_State* state, long value)
    {
        lua_pushinteger(state, value);
    }
    /**
     *  Contains all the logic of how a Lua number is converted to  C++   long
     */
    static long get(lua_State* state)
    {
        return static_cast<long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for const unsigned  long
 */
template <>
struct lua_value<const unsigned long> {
    /**
     *  Contains all the logic of how a C++  const unsigned long is inserted in
     * the lua stack
     */
    static void insert(lua_State* state, const unsigned long& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++
     * unsigned long
     */
    static unsigned long get(lua_State* state)
    {
        return static_cast<unsigned long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for unsigned  long
 */
template <>
struct lua_value<unsigned long> {
    /**
     *  Contains all the logic of how a C++  unsigned long is inserted in the
     * lua stack
     */
    static void insert(lua_State* state, unsigned long value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++
     * unsigned long
     */
    static unsigned long get(lua_State* state)
    {
        return static_cast<unsigned long>(luaL_checklong(state, -1));
    }
};
/**
 * \brief Specialization for const  int
 */
template <>
struct lua_value<const int> {
    /**
     *  Contains all the logic of how a C++  const int is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, const int& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++   int
     */
    static int get(lua_State* state)
    {
        return static_cast<int>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for  int
 */
template <>
struct lua_value<int> {
    /**
     *  Contains all the logic of how a C++ int is inserted in the lua stack
     */
    static void insert(lua_State* state, int value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++   int
     */
    static int get(lua_State* state)
    {
        return static_cast<int>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for const unsigned int
 */
template <>
struct lua_value<const unsigned int> {
    /**
     *  Contains all the logic of how a C++ const unsigned int is inserted in
     * the lua stack
     */
    static void insert(lua_State* state, const unsigned int& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  unsigned
     * int
     */
    static unsigned int get(lua_State* state)
    {
        return static_cast<unsigned int>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for unsigned int
 */
template <>
struct lua_value<unsigned int> {
    /**
     *  Contains all the logic of how a C++  unsigned int is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, unsigned int value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  unsigned
     * int
     */
    static unsigned int get(lua_State* state)
    {
        return static_cast<unsigned int>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for const  short
 */
template <>
struct lua_value<const short> {
    /**
     *  Contains all the logic of how a C++  const  short is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, const short& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  short
     */
    static short get(lua_State* state)
    {
        return static_cast<short>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for  short
 */
template <>
struct lua_value<short> {
    /**
     *  Contains all the logic of how a C++ short is inserted in the lua stack
     */
    static void insert(lua_State* state, short value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  short
     */
    static short get(lua_State* state)
    {
        return static_cast<short>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for const unsigned int
 */
template <>
struct lua_value<const unsigned short> {
    /**
     *  Contains all the logic of how a C++ const unsigned short is inserted in
     * the lua stack
     */
    static void insert(lua_State* state, const unsigned short& value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  unsigned
     * short
     */
    static unsigned short get(lua_State* state)
    {
        return static_cast<unsigned short>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for unsigned int
 */
template <>
struct lua_value<unsigned short> {
    /**
     *  Contains all the logic of how a C++  unsigned short is inserted in the
     * lua stack
     */
    static void insert(lua_State* state, unsigned short value)
    {
        lua_pushinteger(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  unsigned
     * short
     */
    static unsigned short get(lua_State* state)
    {
        return static_cast<unsigned short>(luaL_checkint(state, -1));
    }
};
/**
 * \brief Specialization for const float
 */
template <>
struct lua_value<const float> {
    /**
     *  Contains all the logic of how a C++  const float is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, const float& value)
    {
        lua_pushnumber(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  float
     */
    static float get(lua_State* state)
    {
        return static_cast<float>(lua_tonumber(state, -1));
    }
};
/**
 * \brief Specialization for float
 */
template <>
struct lua_value<float> {
    /**
     *  Contains all the logic of how a C++  float is inserted in the lua stack
     */
    static void insert(lua_State* state, float value)
    {
        lua_pushnumber(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  float
     */
    static float get(lua_State* state)
    {
        return static_cast<float>(lua_tonumber(state, -1));
    }
};
/**
 * \brief Specialization for const double
 */
template <>
struct lua_value<const double> {
    /**
     *  Contains all the logic of how a C++  const double is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, const double& value)
    {
        lua_pushnumber(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  double
     */
    static double get(lua_State* state)
    {
        return static_cast<double>(lua_tonumber(state, -1));
    }
};
/**
 * \brief Specialization for double
 */
template <>
struct lua_value<double> {
    /**
     *  Contains all the logic of how a C++ double is inserted in the lua stack
     */
    static void insert(lua_State* state, double value)
    {
        lua_pushnumber(state, value);
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  double
     */
    static double get(lua_State* state)
    {
        return static_cast<double>(lua_tonumber(state, -1));
    }
};
/**
 * \brief Specialization for std::nullptr_t, contains only insert and inserts
 * the value to nil \note This specialization does not provide get function, the
 * main use of this specialization is to invalidate a lua value\m We use nullptr
 * as substitute of nil
 */
template <>
struct lua_value<std::nullptr_t> {
    /**
     *  Contains all the logic of how a C++ nullptr is inserted in the lua stack
     */
    static void insert(lua_State* state, std::nullptr_t) { lua_pushnil(state); }
};
/**
 * \brief Specialization for const std::string
 */
template <>
struct lua_value<const std::string> {
    /**
     *  Contains all the logic of how a C++ const std::string is inserted in the
     * lua stack
     */
    static void insert(lua_State* state, const std::string& value)
    {
        lua_pushlstring(state, value.c_str(), value.size());
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++
     * std::string
     */
    static std::string get(lua_State* state)
    {
        return static_cast<std::string>(lua_tostring(state, -1));
    }
};
/**
 * \brief Specialization for std::string
 */
template <>
struct lua_value<std::string> {
    /**
     *  Contains all the logic of how a C++  std::string is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, std::string value)
    {
        lua_pushlstring(state, value.c_str(), value.size());
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++
     * std::string
     */
    static std::string get(lua_State* state)
    {
        return static_cast<std::string>(lua_tostring(state, -1));
    }
};
/**
 * \brief Specialization for const char
 */
template <>
struct lua_value<const char> {
    /**
     *  Contains all the logic of how a C++  const char  is inserted in the lua
     * stack
     */
    static void insert(lua_State* state, const char& value)
    {
        lua_value<const std::string>::insert(state, std::string{value});
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  char
     */
    static char get(lua_State* state)
    {
        return lua_value<const std::string>::get(state)[0];
    }
};
/**
 * \brief Specialization for  char
 */
template <>
struct lua_value<char> {
    /**
     *  Contains all the logic of how a C++   char  is inserted in the lua stack
     */
    static void insert(lua_State* state, char value)
    {
        lua_value<std::string>::insert(state, std::string{value});
    }
    /**
     * Contains all the logic of how a Lua number is converted to  C++  char
     */
    static char get(lua_State* state)
    {
        return lua_value<std::string>::get(state)[0];
    }
};
}  // namespace detail
}  // namespace luabz
#endif