#ifndef LUABZ_LUA_ERROR_HPP
#define LUABZ_LUA_ERROR_HPP
#include <string>
namespace luabz
{
namespace detail
{
/**
 * \brief Reports an error generated in lua
 *
 * It handles errors in two different ways \n
 * 1 - When the compiler flag USE_CPP_EXCEPTIONS is set on, it throws an
 * exception whenever there is an error \n 2 - When the compiler flag
 * USE_CPP_EXCEPTIONS is set off (default), it logs the error in a txt file
 * called lua_error and executes an always false assert
 */
void lua_error(const std::string& error_message);
}  // namespace detail
}  // namespace luabz
#endif