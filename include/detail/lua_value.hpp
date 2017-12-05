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
 *
 */
template <class T>
struct lua_value;

/**
 * Specialization for const bool
 */
template <>
struct lua_value<const bool> {
    static void set(lua_State* state,
                    const std::string& name,
                    const bool& value)
    {
        lua_pushboolean(state, value);
        lua_setglobal(state, name.c_str());
    }
    static bool get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<bool>(lua_toboolean(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for bool
 */
template <>
struct lua_value<bool> {
    static void set(lua_State* state, const std::string& name, bool value)
    {
        lua_pushboolean(state, value);
        lua_setglobal(state, name.c_str());
    }
    static bool get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        lua_getglobal(state, name.c_str());
        auto result = static_cast<bool>(lua_toboolean(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const long long
 */
template <>
struct lua_value<const long long> {
    static void set(lua_State* state,
                    const std::string& name,
                    const long long& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static long long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<long long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for long long
 */
template <>
struct lua_value<long long> {
    static void set(lua_State* state, const std::string& name, long long value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static long long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<long long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const unsigned long long
 */
template <>
struct lua_value<const unsigned long long> {
    static void set(lua_State* state,
                    const std::string& name,
                    const unsigned long long& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned long long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result =
            static_cast<unsigned long long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for unsigned long long
 */
template <>
struct lua_value<unsigned long long> {
    static void set(lua_State* state,
                    const std::string& name,
                    unsigned long long value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned long long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result =
            static_cast<unsigned long long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const  long
 */
template <>
struct lua_value<const long> {
    static void set(lua_State* state,
                    const std::string& name,
                    const long& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for  long
 */
template <>
struct lua_value<long> {
    static void set(lua_State* state, const std::string& name, long value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const unsigned  long
 */
template <>
struct lua_value<const unsigned long> {
    static void set(lua_State* state,
                    const std::string& name,
                    const unsigned long& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<unsigned long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for unsigned  long
 */
template <>
struct lua_value<unsigned long> {
    static void set(lua_State* state,
                    const std::string& name,
                    unsigned long value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned long get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<unsigned long>(luaL_checklong(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const  int
 */
template <>
struct lua_value<const int> {
    static void set(lua_State* state, const std::string& name, const int& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static int get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<int>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for  int
 */
template <>
struct lua_value<int> {
    static void set(lua_State* state, const std::string& name, int value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static int get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<int>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const unsigned int
 */
template <>
struct lua_value<const unsigned int> {
    static void set(lua_State* state,
                    const std::string& name,
                    const unsigned int& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned int get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<unsigned int>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for unsigned int
 */
template <>
struct lua_value<unsigned int> {
    static void set(lua_State* state,
                    const std::string& name,
                    unsigned int value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned int get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<unsigned int>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const  short
 */
template <>
struct lua_value<const short> {
    static void set(lua_State* state,
                    const std::string& name,
                    const short& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static short get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<short>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for  short
 */
template <>
struct lua_value<short> {
    static void set(lua_State* state, const std::string& name, short value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static short get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<short>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const unsigned int
 */
template <>
struct lua_value<const unsigned short> {
    static void set(lua_State* state,
                    const std::string& name,
                    const unsigned short& value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned short get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<unsigned short>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for unsigned int
 */
template <>
struct lua_value<unsigned short> {
    static void set(lua_State* state,
                    const std::string& name,
                    unsigned short value)
    {
        lua_pushinteger(state, value);
        lua_setglobal(state, name.c_str());
    }
    static unsigned short get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<unsigned short>(luaL_checkint(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const float
 */
template <>
struct lua_value<const float> {
    static void set(lua_State* state,
                    const std::string& name,
                    const float& value)
    {
        lua_pushnumber(state, value);
        lua_setglobal(state, name.c_str());
    }
    static float get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<float>(lua_tonumber(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for float
 */
template <>
struct lua_value<float> {
    static void set(lua_State* state, const std::string& name, float value)
    {
        lua_pushnumber(state, value);
        lua_setglobal(state, name.c_str());
    }
    static float get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<float>(lua_tonumber(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const double
 */
template <>
struct lua_value<const double> {
    static void set(lua_State* state,
                    const std::string& name,
                    const double& value)
    {
        lua_pushnumber(state, value);
        lua_setglobal(state, name.c_str());
    }
    static double get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<double>(lua_tonumber(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for double
 */
template <>
struct lua_value<double> {
    static void set(lua_State* state, const std::string& name, double value)
    {
        lua_pushnumber(state, value);
        lua_setglobal(state, name.c_str());
    }
    static double get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<double>(lua_tonumber(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for std::nullptr_t, contains only set and sets the value to
 * nil
 */
template <>
struct lua_value<std::nullptr_t> {
    static void set(lua_State* state, const std::string& name, std::nullptr_t)
    {
        lua_pushnil(state);
        lua_setglobal(state, name.c_str());
    }
};
/**
 * Specialization for const std::string
 */
template <>
struct lua_value<const std::string> {
    static void set(lua_State* state,
                    const std::string& name,
                    const std::string& value)
    {
        lua_pushlstring(state, value.c_str(), value.size());
        lua_setglobal(state, name.c_str());
    }
    static std::string get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<std::string>(lua_tostring(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for std::string
 */
template <>
struct lua_value<std::string> {
    static void set(lua_State* state,
                    const std::string& name,
                    std::string value)
    {
        lua_pushlstring(state, value.c_str(), value.size());
        lua_setglobal(state, name.c_str());
    }
    static std::string get(lua_State* state, const std::string& name)
    {
        lua_getglobal(state, name.c_str());
        auto result = static_cast<std::string>(lua_tostring(state, -1));
        lua_pop(state, 1);
        return result;
    }
};
/**
 * Specialization for const char
 */
template <>
struct lua_value<const char> {
    static void set(lua_State* state,
                    const std::string& name,
                    const char& value)
    {
        lua_value<const std::string>::set(state, name, std::string{value});
    }
    static char get(lua_State* state, const std::string& name)
    {
        return lua_value<const std::string>::get(state, name)[0];
    }
};
/**
 * Specialization for  char
 */
template <>
struct lua_value<char> {
    static void set(lua_State* state, const std::string& name, char value)
    {
        lua_value<std::string>::set(state, name, std::string{value});
    }
    static char get(lua_State* state, const std::string& name)
    {
        return lua_value<std::string>::get(state, name)[0];
    }
};
}
}
#endif