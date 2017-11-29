#include <cassert>
#include <fstream>
#include "core/lua_exception.hpp"
#include "detail/lua_error.hpp"
namespace LuaBz
{
namespace detail
{
void lua_error(const std::string &message)
{
#ifdef USE_CPP_EXCEPTIONS
    throw lua_exception(message);
#else
    std::ofstream error_log("lua_error.txt", std::ios::app);
    error_log << error_message << "\n";
    error_log.close();
    assert(false && "A lua error was encountered, please check the "
                    "lua_error.txt file in order to find out more information");
#endif
}
}
}