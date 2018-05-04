#pragma once
#include "error.hpp"
#include <lua.hpp>
#include <string>
namespace luabz
{
/**
 * \brief RAII helper used to push to the top of the stack a specific
 * lua_variable
 */
class var_loader
{
  public:
    /**
     * Pushes to the top of the stack a specific lua variable
     * \param st The state in which the variable is present
     * \param variable_name The name of the lua variable
     * \param us Starting used stack space
     */
    var_loader(lua_State* st, const std::string& variable_name) : state{st}, used_space{0}
    {
        if (variable_name.find('.') != std::string::npos) {
            auto delimeter_position = variable_name.find_first_of('.');
            std::string table_name = variable_name.substr(0, delimeter_position);
            std::string field_name = variable_name.substr(delimeter_position + 1);
            lua_getglobal(state, table_name.c_str());
            ++used_space;
            while (true) {
                delimeter_position = field_name.find_first_of('.');
                bool field_reached = (delimeter_position == std::string::npos);
                if (field_reached) {
                    lua_getfield(state, -1, field_name.c_str());
                    ++used_space;
                    return;
                }
                std::string parent_field_name = field_name.substr(0, delimeter_position);
                field_name = field_name.substr(delimeter_position + 1);
                lua_getfield(state, -1, parent_field_name.c_str());
                ++used_space;
                if (!lua_istable(state, -1)) {
                    std::string error_message = parent_field_name;
                    error_message += " is not a table, and cannot contain the "
                                     "following  fields ";
                    error_message += field_name;
                    error_message += std::to_string(used_space);
                    error_message += "\nOriginal string: ";
                    error_message += variable_name;
                    error_message += "\n";
                    error(error_message);
                }
            }
        }
        lua_getfield(state, LUA_GLOBALSINDEX, variable_name.c_str());
        ++used_space;
    }

    ~var_loader() { lua_pop(state, (used_space)); }
    // TODO:Find better way of doing this
    void increment_used_space() { ++used_space; }

  private:
    lua_State* state;

    int used_space;
};
}  // namespace luabz