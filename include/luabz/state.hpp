#pragma once
#include "error.hpp"
#include <functional>
#include <lua.hpp>
#include <unordered_map>
#include <vector>
namespace luabz
{
class state
{
  public:
    /// Generates new lua_state to be used by script
    /**
     *
     * \param file_name The name of the lua file on which the new lua state
     * \param load_std Whether or not to load lua's std
     * operates
     * \note If there is an already opened lua state associated with file_name
     * thats the one thats returned, otherwise a new lua state is created
     */
    // TODO:Rename and split this shit
    static lua_State* get(const std::string& file_name, bool load_std = false)
    {
        bool already_inserted =
            get_active_lua_states().find(file_name) != get_active_lua_states().end();
        if (already_inserted) {
            return get_loaded_lua_state(file_name);
        }
        return create_new_lua_state(file_name, load_std);
    }

    static void open_std(const std::string& file_name)
    {
        lua_State* m_state = get_loaded_lua_state(file_name);
        luaL_openlibs(m_state);
    }

    static void close(const std::string& file_name)
    {
        lua_State* state = get_loaded_lua_state(file_name);
        lua_close(state);
        state = nullptr;
        get_active_lua_states()[file_name] = state;
        get_active_lua_states().erase(file_name);
    }

    static std::function<int(lua_State*)> get_cpp_registered_function(std::size_t index)
    {
        return get_all_registered_functions().at(index);
    }

    static void remove_cpp_registered_functions(lua_State* state)
    {
        auto file_registered_functions = get_cpp_registered_functions(state);
        file_registered_functions.clear();
    }
    static std::size_t register_cpp_function(lua_State* state, std::function<int(lua_State*)>&& f)
    {
        get_all_registered_functions().emplace_back(f);
        auto inserted_function_position = get_all_registered_functions().size() - 1;
        get_cpp_registered_functions(state).push_back(inserted_function_position);
        return inserted_function_position;
    }

  private:
    static std::vector<int>& get_cpp_registered_functions(lua_State* state)
    {
        static std::unordered_map<lua_State*, std::vector<int>> cpp_registered_functions{};
        return cpp_registered_functions[state];
    }
    static std::vector<std::function<int(lua_State*)>>& get_all_registered_functions()
    {
        static std::vector<std::function<int(lua_State*)>> registered_functions{};
        return registered_functions;
    }
    /**
     * \brief Retrieves the lua script associated with file_name from
     * from the collection of opened lua states
     */
    static lua_State* get_loaded_lua_state(const std::string& file_name)
    {
        return get_active_lua_states()[file_name];
    }

    static lua_State* create_new_lua_state(const std::string& file_name, bool load_std)
    {
        lua_State* state = luaL_newstate();
        if (state == nullptr) {
            error("Cannot create a new lua_State");
        }
        if (luaL_dofile(state, file_name.c_str())) {
            error(lua_tostring(state, -1));
        }
        if (load_std) {
            luaL_openlibs(state);
        }
        get_active_lua_states()[file_name] = state;
        return state;
    }

    static std::unordered_map<std::string, lua_State*>& get_active_lua_states()
    {
        static std::unordered_map<std::string, lua_State*> active_lua_states{};
        return active_lua_states;
    }
};

}  // namespace luabz