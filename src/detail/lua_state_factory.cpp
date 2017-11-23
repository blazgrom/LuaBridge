#include "detail/lua_state_factory.hpp"
namespace LuaBz
{
namespace detail
{
/**
 * \todo \n
 * 1-When the file has already been loaded into a lua thread we should \n
 * retrieve this lua thread from master state's registry, see line 17
 */
lua_State *lua_state_factory::create_state(const std::string &file_name)
{
    static std::unordered_map<std::string, int> active_lua_states;
    static lua_State *master_state = luaL_newstate();
    const std::string registry_key = "luabz_" + file_name;
    bool already_opened =
        active_lua_states.find(registry_key) != active_lua_states.end();
    if (already_opened) {
        lua_rawgeti(master_state, LUA_REGISTRYINDEX,
                    active_lua_states[registry_key]);
        return lua_tothread(master_state, -1);
    }
    lua_State *state = lua_newthread(master_state);
    auto lua_ref = luaL_ref(master_state, LUA_REGISTRYINDEX);
    active_lua_states[registry_key] = lua_ref;
    set_state_globaltable(state);
    luaL_dofile(state, file_name.c_str());
    return state;
}
void lua_state_factory::set_state_globaltable(lua_State *state)
{
#if LUA_VERSION_NUM <= 501
    auto old_globaltable_index = LUA_GLOBALSINDEX;
#else
    auto old_global_TABLE_INDEX = LUA_RIDX_GLOBALS;
#endif
    // Insert the new global table
    lua_newtable(state);
    // Set the new global table
    lua_replace(state, LUA_GLOBALSINDEX);
}
}
}