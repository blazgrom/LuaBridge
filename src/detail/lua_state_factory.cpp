#include "detail/lua_state_factory.hpp"
#include <unordered_map>
#include "detail/lua_error.hpp"
namespace luabz
{
namespace detail
{
static std::unordered_map<std::string, int> active_lua_states;
static lua_State* master_state = luaL_newstate();
/**
 * \todo \n
 * 1-When the file has already been loaded into a lua thread we should \n
 * retrieve this lua thread from master state's registry, see line 17
 *
 * \todo Implement load of lua std
 */

lua_State* lua_state_factory::create_state(const std::string& file_name,
                                           bool load_std)
{
    bool already_inserted =
        active_lua_states.find(file_name) != active_lua_states.end();
    if (already_inserted) {
        auto lua_ref = active_lua_states[file_name];
        lua_rawgeti(master_state, LUA_REGISTRYINDEX, lua_ref);
        if (lua_isthread(master_state, -1)) {
            lua_State* state = lua_tothread(master_state, -1);
            lua_pop(master_state, -1);
            return state;
        }
        lua_error("Lua slave state not found");
    }
    lua_State* state = lua_newthread(master_state);
    auto lua_ref = luaL_ref(master_state, LUA_REGISTRYINDEX);
    active_lua_states.emplace(file_name, lua_ref);
    set_state_globaltable(state);
    if (luaL_dofile(state, file_name.c_str())) {
        lua_error(lua_tostring(state, -1));
    }
    return state;
}
void lua_state_factory::set_state_globaltable(lua_State* state)
{
    // Insert the new global table
    lua_newtable(state);
    // Set the new global table
    lua_replace(state, LUA_GLOBALSINDEX);
}
}  // namespace detail
}  // namespace luabz