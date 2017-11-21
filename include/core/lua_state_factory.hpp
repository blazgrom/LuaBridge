#ifndef LUABZ_LUA_STATE_FACTORY_HPP
#define LUABZ_LUA_STATE_FACTORY_HPP

#include <string>
#include <unordered_map>
#include <lua.hpp>

namespace LuaBz
{
    class lua_state_factory
    {
        public:
        ///Generates new lua_state to be used by lua script
        /**
         * 
         * \note Every new lua_thread is pushed onto the registry in order \n
         * to not allow GC from Lua
         * \todo  \n
         * Check if when opening two different script files they have common globals \n
         * Every file should have it's own globals
         * 
         */
        static lua_State* create_state(const std::string& file_name)
        {
            static std::unordered_map<std::string,int> active_lua_states;
            static lua_State* master_state=luaL_newstate();
            const std::string registry_key="luabz_"+file_name;
            if(active_lua_states.find(file_registry_key))
            {
                return active_lua_states[registry_key];
            }
            lua_State* state=lua_newthread(master_state);
            auto lua_ref=luaL_ref(master_state, registry_key);
            active_lua_states[registry_key]=lua_ref;
            set_state_globaltable(state);
            state=luaL_dofile(state, file_name.c_str());
            return state;            
        }
        private:
        ///Sets the global table of the new state
        /**
         * \note Has the following logic \n
         * 1 ) Insert the new global table and the metatable of the former
         * 2 ) Set the __index metamethod of the metatable to the old global table
         * 3 ) Set the metatable of the new global replace
         * 4 ) Replace the current global table with the new global table
         * \todo LUA_GLOBALSINDEX is not supported from Lua5.1 add preprocessor checks
        */
        static void set_state_globaltable(lua_State* state)
        {
            #if LUA_VERSION_NUM<=501
            auto old_globaltable_index=LUA_GLOBALSINDEX;
            #else 
            auto old_global_TABLE_INDEX=LUA_RIDX_GLOBALS;
            #endif
            //Insert the new global table
            lua_newtable(state);
            //Create the new metatable
            //The __index of this table points to the old LUA_GLOBALSINDEX,
            //in order to be able to retrieve value already defined.
            //New values should be inserted in the new global table
            lua_newtable(state);
            lua_pushliteral(state,"__index");
            lua_pushvalue(state,old_globaltable_index);
            static constexpr int new_metable_index=-3;
            lua_settable(state,new_metable_index);
            //Set the metable of the new global table
            static constexpr int new_globaltable_index=-2;
            lua_setmetatable(state,old_globaltable_index);
            //Set the new global table
            lua_replace(state,LUA_GLOBALSINDEX);
        }
    };
}
#endif