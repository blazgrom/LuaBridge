#pragma once
#include "interface.hpp"
#include <string>
namespace luabz
{
/**
 * \brief The main class used for retrieving/inserting values from/into a lua
 * stack. \pre Every specialization has to provide a static member function
 * called "insert" which accepts \n lua_State* and addional parameter which can
 * be anything. \pre Every specialization has to provice a static member
 * function called "get" accepting lua_State and stack_index and which convert
 * and returns the \n top element of the stack to the desired type. \note If one
 * of the required function is not provided you will not be the corresponding
 * functionality with the new type.
 */
template <typename T>
struct value;

template <>
struct value<bool> {
    static void insert(lua_State* state, bool value) { interface::insert_bool(state, value); }

    static bool get(lua_State* state, int stack_index)
    {
        return static_cast<bool>(interface::get_bool(state, stack_index));
    }
};

template <>
struct value<long long> {
    static void insert(lua_State* state, long long value)
    {
        interface::insert_integer(state, value);
    }

    static long long get(lua_State* state, int stack_index)
    {
        return static_cast<long long>(interface::get_long(state, stack_index));
    }
};

template <>
struct value<unsigned long long> {
    static void insert(lua_State* state, unsigned long long value)
    {
        interface::insert_integer(state, value);
    }

    static unsigned long long get(lua_State* state, int stack_index)
    {
        return static_cast<unsigned long long>(interface::get_long(state, stack_index));
    }
};

template <>
struct value<long> {
    static void insert(lua_State* state, long value) { interface::insert_integer(state, value); }

    static long get(lua_State* state, int stack_index)
    {
        return static_cast<long>(interface::get_long(state, stack_index));
    }
};

template <>
struct value<unsigned long> {
    static void insert(lua_State* state, unsigned long value)
    {
        interface::insert_integer(state, value);
    }

    static unsigned long get(lua_State* state, int stack_index)
    {
        return static_cast<unsigned long>(interface::get_long(state, stack_index));
    }
};

template <>
struct value<int> {
    static void insert(lua_State* state, int value) { interface::insert_integer(state, value); }

    static int get(lua_State* state, int stack_index)
    {
        return static_cast<int>(interface::get_int(state, stack_index));
    }
};

template <>
struct value<unsigned int> {
    static void insert(lua_State* state, unsigned int value)
    {
        interface::insert_integer(state, value);
    }

    static unsigned int get(lua_State* state, int stack_index)
    {
        return static_cast<unsigned int>(interface::get_int(state, stack_index));
    }
};

template <>
struct value<float> {
    static void insert(lua_State* state, float value) { interface::insert_number(state, value); }

    static float get(lua_State* state, int stack_index)
    {
        return static_cast<float>(interface::get_number(state, stack_index));
    }
};

template <>
struct value<double> {
    static void insert(lua_State* state, double value) { interface::insert_number(state, value); }

    static double get(lua_State* state, int stack_index)
    {
        return static_cast<double>(interface::get_number(state, stack_index));
    }
};

template <>
struct value<std::nullptr_t> {
    static void insert(lua_State* state, std::nullptr_t) { lua_pushnil(state); }
};

template <>
struct value<std::string> {
    static void insert(lua_State* state, std::string value)
    {
        lua_pushlstring(state, value.c_str(), value.size());
    }

    static std::string get(lua_State* state, int stack_index)
    {
        return static_cast<std::string>(lua_tostring(state, stack_index));
    }
};

template <>
struct value<char> {
    static void insert(lua_State* state, char value)
    {
        luabz::value<std::string>::insert(state, std::string{value});
    }

    static char get(lua_State* state, int stack_index)
    {
        return luabz::value<std::string>::get(state, stack_index)[0];
    }
};

template <>
struct value<lua_CFunction> {
    static void insert(lua_State* state, lua_CFunction function, unsigned int upvalues_count)
    {
        interface::insert_function(state, function, upvalues_count);
    }
};
}  // namespace luabz