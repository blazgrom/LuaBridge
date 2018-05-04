#pragma once
#include "luabz_exception.hpp"
#include <cstdio>
#include <exception>
#include <string>
namespace luabz
{
/**
 * \brief Reports an error generated in lua
 *
 * It handles errors in two different ways \n
 * 1 - When the compiler flag USE_CPP_EXCEPTIONS is set on, it throws an
 * exception whenever there is an error \n 2 - When the compiler flag
 * USE_CPP_EXCEPTIONS is set off (default), it logs the error in a txt file
 * called error and executes an always false assert
 */
//TODO:Improve error handling
inline void error(const std::string& error_message)
{
#ifdef USE_CPP_EXCEPTIONS
    throw luabz_exception(message);
#else
    std::printf("%s",error_message.c_str());
   // std::terminate();
#endif
}
}  // namespace luabz