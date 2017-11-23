#ifndef LUABZ_LUA_ERROR_HPP
#define LUABZ_LUA_ERROR_HPP
#include <stdexcept>
#include <string>
namespace LuaBz
{
class lua_error : public std::runtime_error
{
  public:
    explicit lua_error(const std::string &msg);
    explicit lua_error(const char *msg);

  private:
};
}
#endif  // !LUABZ_LUA_ERROR_HPP
