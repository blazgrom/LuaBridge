#ifndef LUABZ_LUA_STATE_FACTORY_HPP
#define LUABZ_LUA_STATE_FACTORY_HPP

#include <lua.hpp>
#include <string>
#include <unordered_map>

namespace luabz
{
namespace detail
{
/**
 * \brief A factory used to generate/retrieve new lua_State*
 *
 * The generation of new lua states uses the following logic.
 * For each lua file we want to open we create a new lua state. The state
 * is closed only when explicitly required by the lua_script object, this permits us to 
 * share the same lua file between multiple lua_script object.
 */
class lua_state_factory
{
  public:
    /// Generates new lua_state to be used by lua_script
    /**
     *
     * \param file_name The name of the lua file on which the new lua state
     * \param load_std Whether or not to load lua's std
     * operates
     * \note If there is an already opened lua state associated with file_name thats the one thats
     * returned, otherwise a new lua state is created
     */
    static lua_State* get_lua_state(const std::string& file_name,
                                    bool load_std = false);
    /**
     * \brief Open all standard library on a lua file
     * \param file_name The name of the lua file
     * \pre There must be an already opened lua state asssociated with the
     * file_name
     * \param file_name The name of the lua file inside which we want to open the standard library
     */
    static void open_standard_library(const std::string& file_name);
    /**
     * \brief Closes the lua_State associated with file_name, thus destroying all object in
     * the givedn lua_State and frees all dynamica memory used by the state
     * \pre There must be an already opened lua state asscociated with the file_name
     * \param file_name The name of the file for which we want to close the lua state
    */
    static void close_lua_state(const std::string& file_name);

  private:
    /**
     * \brief Retrieves the lua script associated with file_name from
     * from the collection of opened lua states
     */
    static lua_State* get_loaded_lua_state(const std::string& file_name);
    /**
     * \brief Creates new lua_State which is associated with the script file
     */
    static lua_State* create_new_lua_state(const std::string& file_name,
                                           bool load_std);
};
}  // namespace detail
}  // namespace luabz
#endif