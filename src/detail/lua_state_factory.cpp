#include "detail/lua_state_factory.hpp"
#include "detail/lua_error.hpp"
#include <unordered_map>
namespace luabz
{
namespace detail
{
static std::unordered_map<std::string, lua_State*>
    active_lua_states;  ///< Contains
                        ///< all the lua
                        ///< states
                        ///< connected
                        ///< to an
                        ///< already
                        ///< open lua
                        ///< file
lua_State* lua_state_factory::get_loaded_lua_state(const std::string& file_name)
{
    return active_lua_states[file_name];
}

lua_State* lua_state_factory::create_new_lua_state(const std::string& file_name,
                                                   bool load_std)
{
    lua_State* state = luaL_newstate();
    if (state == nullptr) {
        lua_error("Cannot create a new lua_State");
    }
    if (luaL_dofile(state, file_name.c_str())) {
        lua_error(lua_tostring(state, -1));
    }
    if (load_std) {
        luaL_openlibs(state);
    }
    active_lua_states[file_name] = state;
    return state;
}
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
void lua_state_factory::open_standard_library(const std::string& file_name)
{
    lua_State* m_state = get_loaded_lua_state(file_name);
    luaL_openlibs(m_state);
}
void lua_state_factory::close_lua_state(const std::string& file_name)
{
    lua_State* state = get_loaded_lua_state(file_name);
    lua_close(state);
    active_lua_states.erase(file_name);
}

}  // namespace detail
}  // namespace luabz