#ifndef LUABZ_LUA_VALUE_HPP
#define LUABZ_LUA_VALUE_HPP
#include <lua.hpp>
#include <string>
namespace luabz
{
namespace detail
{
/**
 * Struct containing all the logic for extracting value from lua and inserting
 * value in lua
 * \todo Rename insert to something like insert or something
 * Decide if get has to be renamed also
 */
template <class T>
struct lua_value;

/**
 * Specialization for const bool
 */
template <>
struct lua_value<const bool> {
    static void insert(lua_State* state,
                    const bool& value)
    {
        lua_pushboolean(state, value);
    }
    static bool get(lua_State* state)
    {
        return static_cast<bool>(lua_toboolean(state, -1));
    }
};
/**
 * Specialization for bool
 */
template <>
struct lua_value<bool> {
    static void insert(lua_State* state, bool value)
    {
        lua_pushboolean(state, value);
    }
    static bool get(lua_State* state)
    {
        return static_cast<bool>(lua_toboolean(state, -1));
    }
};
/**
 * Specialization for const long long
 */
template <>
struct lua_value<const long long> {
    static void insert(lua_State* state,
                    const long long& value)
    {
        lua_pushinteger(state, value);
    }
    static long long get(lua_State* state)
    {
        return static_cast<long long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for long long
 */
template <>
struct lua_value<long long> {
    static void insert(lua_State* state, long long value)
    {
        lua_pushinteger(state, value);
    }
    static long long get(lua_State* state)
    {
        return static_cast<long long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for const unsigned long long
 */
template <>
struct lua_value<const unsigned long long> {
    static void insert(lua_State* state,
                    const unsigned long long& value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned long long get(lua_State* state)
    {
        return static_cast<unsigned long long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for unsigned long long
 */
template <>
struct lua_value<unsigned long long> {
    static void insert(lua_State* state,
                    unsigned long long value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned long long get(lua_State* state)
    {
        return static_cast<unsigned long long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for const  long
 */
template <>
struct lua_value<const long> {
    static void insert(lua_State* state,
                    const long& value)
    {
        lua_pushinteger(state, value);
    }
    static long get(lua_State* state)
    {
        return static_cast<long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for  long
 */
template <>
struct lua_value<long> {
    static void insert(lua_State* state, long value)
    {
        lua_pushinteger(state, value);
    }
    static long get(lua_State* state)
    {
        return static_cast<long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for const unsigned  long
 */
template <>
struct lua_value<const unsigned long> {
    static void insert(lua_State* state,
                    const unsigned long& value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned long get(lua_State* state)
    {
        return static_cast<unsigned long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for unsigned  long
 */
template <>
struct lua_value<unsigned long> {
    static void insert(lua_State* state,
                    unsigned long value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned long get(lua_State* state)
    {
        return static_cast<unsigned long>(luaL_checklong(state, -1));
    }
};
/**
 * Specialization for const  int
 */
template <>
struct lua_value<const int> {
    static void insert(lua_State* state, const int& value)
    {
        lua_pushinteger(state, value);
    }
    static int get(lua_State* state)
    {
        return static_cast<int>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for  int
 */
template <>
struct lua_value<int> {
    static void insert(lua_State* state, int value)
    {
        lua_pushinteger(state, value);
    }
    static int get(lua_State* state)
    {
        return static_cast<int>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for const unsigned int
 */
template <>
struct lua_value<const unsigned int> {
    static void insert(lua_State* state,
                    const unsigned int& value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned int get(lua_State* state)
    {
        return static_cast<unsigned int>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for unsigned int
 */
template <>
struct lua_value<unsigned int> {
    static void insert(lua_State* state,
                    unsigned int value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned int get(lua_State* state)
    {
        return static_cast<unsigned int>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for const  short
 */
template <>
struct lua_value<const short> {
    static void insert(lua_State* state,
                    const short& value)
    {
        lua_pushinteger(state, value);
    }
    static short get(lua_State* state)
    {
        return static_cast<short>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for  short
 */
template <>
struct lua_value<short> {
    static void insert(lua_State* state,  short value)
    {
        lua_pushinteger(state, value);
    }
    static short get(lua_State* state)
    {
        return static_cast<short>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for const unsigned int
 */
template <>
struct lua_value<const unsigned short> {
    static void insert(lua_State* state,
                    const unsigned short& value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned short get(lua_State* state)
    {
        return static_cast<unsigned short>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for unsigned int
 */
template <>
struct lua_value<unsigned short> {
    static void insert(lua_State* state,
                    unsigned short value)
    {
        lua_pushinteger(state, value);
    }
    static unsigned short get(lua_State* state)
    {
        return static_cast<unsigned short>(luaL_checkint(state, -1));
    }
};
/**
 * Specialization for const float
 */
template <>
struct lua_value<const float> {
    static void insert(lua_State* state,
                    const float& value)
    {
        lua_pushnumber(state, value);
    }
    static float get(lua_State* state)
    {
        return static_cast<float>(lua_tonumber(state, -1));
    }
};
/**
 * Specialization for float
 */
template <>
struct lua_value<float> {
    static void insert(lua_State* state, float value)
    {
        lua_pushnumber(state, value);
    }
    static float get(lua_State* state)
    {
        return static_cast<float>(lua_tonumber(state, -1));
    }
};
/**
 * Specialization for const double
 */
template <>
struct lua_value<const double> {
    static void insert(lua_State* state,
                    const double& value)
    {
        lua_pushnumber(state, value);
    }
    static double get(lua_State* state)
    {
        return static_cast<double>(lua_tonumber(state, -1));
    }
};
/**
 * Specialization for double
 */
template <>
struct lua_value<double> {
    static void insert(lua_State* state, double value)
    {
        lua_pushnumber(state, value);
    }
    static double get(lua_State* state)
    {
        return static_cast<double>(lua_tonumber(state, -1));
    }
};
/**
 * Specialization for std::nullptr_t, contains only insert and inserts the value to
 * nil
 */
template <>
struct lua_value<std::nullptr_t> {
    static void insert(lua_State* state, std::nullptr_t)
    {
        lua_pushnil(state);
    }
};
/**
 * Specialization for const std::string
 */
template <>
struct lua_value<const std::string> {
    static void insert(lua_State* state,
                    const std::string& value)
    {
        lua_pushlstring(state, value.c_str(), value.size());
    }
    static std::string get(lua_State* state)
    {
        return static_cast<std::string>(lua_tostring(state, -1));
    }
};
/**
 * Specialization for std::string
 */
template <>
struct lua_value<std::string> {
    static void insert(lua_State* state,
                    std::string value)
    {
        lua_pushlstring(state, value.c_str(), value.size());
    }
    static std::string get(lua_State* state)
    {
        return static_cast<std::string>(lua_tostring(state, -1));
    }
};
/**
 * Specialization for const char
 */
template <>
struct lua_value<const char> {
    static void insert(lua_State* state,
                    const char& value)
    {
        lua_value<const std::string>::insert(state,  std::string{value});
    }
    static char get(lua_State* state)
    {
        return lua_value<const std::string>::get(state)[0];
    }
};
/**
 * Specialization for  char
 */
template <>
struct lua_value<char> {
    static void insert(lua_State* state,  char value)
    {
        lua_value<std::string>::insert(state, std::string{value});
    }
    static char get(lua_State* state)
    {
        return lua_value<std::string>::get(state)[0];
    }
};
}
}
#endif