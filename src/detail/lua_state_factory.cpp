#include "detail/lua_error.hpp"
#include "detail/lua_state_factory.hpp"
#include <unordered_map>
namespace luabz
{
namespace detail
{
static std::unordered_map<std::string, int> active_lua_states;  ///< Contains
                                                                ///< all the lua
                                                                ///< states
                                                                ///< connected
                                                                ///< to an
                                                                ///< already
                                                                ///< open lua
                                                                ///< file
static lua_State* master_state =
    luaL_newstate();  ///< The master lua state, which repository is used to
                      ///< store the other lua states. This state is necessary
                      ///< because it's the only one that lua will not
                      ///< automatically collect, so it's used to store the
                      ///< other lua states

lua_State* lua_state_factory::get_loaded_lua_state(const std::string& file_name)
{
    auto lua_ref = active_lua_states[file_name];
    lua_rawgeti(master_state, LUA_REGISTRYINDEX, lua_ref);
    if (lua_isthread(master_state, -1)) {
        lua_State* state = lua_tothread(master_state, -1);
        lua_pop(master_state, -1);
        return state;
    }
    lua_error("Lua slave state not found");
    return nullptr;
}

lua_State* lua_state_factory::create_new_lua_state(const std::string& file_name,
                                                   bool load_std)
{
    lua_State* state = lua_newthread(master_state);
    auto lua_ref = luaL_ref(master_state, LUA_REGISTRYINDEX);
    active_lua_states.emplace(file_name, lua_ref);
    set_state_globaltable(state);
    if (luaL_dofile(state, file_name.c_str())) {
        lua_error(lua_tostring(state, -1));
    }
    if (load_std) {
        luaL_openlibs(state);
    }
    return state;
}
/**
 * \todo Add tests for checking if loading of lua's standard library works
 */
lua_State* lua_state_factory::get_lua_state(const std::string& file_name,
                                            bool load_std)
{
    bool already_inserted =
        active_lua_states.find(file_name) != active_lua_states.end();
    if (already_inserted) {
        return get_loaded_lua_state(file_name);
    }
    return create_new_lua_state(file_name, load_std);
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