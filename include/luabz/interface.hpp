#pragma once
#include <lua.hpp>
#include <cstdint>
#include <type_traits>

namespace luabz{
    /**
 * \brief Used to interface with lua api
*/
struct interface{
    //TODO:Decide if this should use the checked version or the unchecked once... or both
    static auto get_long(lua_State* state,std::size_t index){
        return luaL_checklong(state,index);
    }
    static auto get_bool(lua_State* state,std::size_t index){
        return lua_toboolean(state,index);
    }
    static auto get_int(lua_State* state,std::size_t index){
        return luaL_checkint(state,index);
    }
    static auto get_number(lua_State* state,std::size_t index){
        return luaL_checknumber(state,index);
    }
    static auto get_string(lua_State* state,std::size_t index){
        return luaL_checkstring(state,index);
    }

    static void insert_bool(lua_State* state,bool value){
        lua_pushboolean(state,value);
    }
    ///TODO:This is missing enable if
    template<typename T>
    static void insert_integer(lua_State* state,T value){
        lua_pushinteger(state,value);
    }
    template<typename T,typename=typename std::enable_if<std::is_floating_point<T>::value>::type>
    static void insert_number(lua_State* state,T value){
        lua_pushnumber(state,value);
    }
    static void insert_function(lua_State* state,lua_CFunction f,std::size_t upvalues_count){
        lua_pushcclosure(state, f, upvalues_count);
    }
};
}
