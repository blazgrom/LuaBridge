#ifndef luabz_LUA_ERROR_HPP
#define luabz_LUA_ERROR_HPP
#include <string>
namespace luabz
{
namespace detail
{
/**
 * \brief Reports an error generated from the lua c api
 *
 * It can handle an error in two different ways \n
 * 1 - When the compiler flag USE_CPP_EXCEPTIONS is set on, it throws an
 * exception whenever there is an error \n 2 - When the compiler flag
 * USE_CPP_EXCEPTIONS is set off (default), it logs the error in a txt file
 * called lua_error and executes an always false assert
 */
void lua_error(const std::string& message);
}
}
#endif