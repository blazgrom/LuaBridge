#ifndef LUABZ_LUA_STATE_FACTORY_HPP
#define LUABZ_LUA_STATE_FACTORY_HPP

#include <lua.hpp>
#include <string>
#include <unordered_map>

namespace LuaBz
{
namespace detail
{
class lua_state_factory
{
  public:
    /// Generates new lua_state to be used by lua_script
    /**
     *
     * \note Every new lua_thread is pushed onto the registry in order \n
     * to not allow GC from Lua
     * \todo maybe rename this function because create_state may be ambiguos
     * seen that the function doesn't create \n a state when we are calling it
     * two times with the same name
     */
    static lua_State *create_state(const std::string &file_name);

  private:
    /// Sets the global table of the new state
    /**
     * \note Has the following logic \n
     * 1 ) Insert the new global table and the metatable of the former
     * 2 ) Replace the current global table with the new global table
     */
    static void set_state_globaltable(lua_State *state);
};
}
}
#endif