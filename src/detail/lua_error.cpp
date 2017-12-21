#include "detail/lua_error.hpp"
#include "lua_exception.hpp"
#include <cassert>
#include <fstream>
namespace luabz
{
namespace detail
{
void lua_error(const std::string& error_message)
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
}  // namespace detail
}  // namespace luabz