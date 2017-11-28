#ifndef LUABZ_LUA_EXCEPTION_HPP
#define LUABZ_LUA_EXCEPTION_HPP
#include <stdexcept>
#include <string>
namespace LuaBz
{
class lua_exception : public std::runtime_error
{
  public:
    lua_exception(const std::string &msg);
    lua_exception(const char *msg);
    const char *what() const noexcept override;
  private:
};
}
#endif  // !LUABZ_LUA_EXCEPTION_HPP
