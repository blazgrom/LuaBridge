#ifndef LUABZ_LUA_EXCEPTION_HPP
#define LUABZ_LUA_EXCEPTION_HPP
#include <stdexcept>
#include <string>
namespace luabz
{
/**
 * \brief A class used to report any kind of error that lua can generate
 *
 * Class used to report all error that can be generated from lua.\n
 * It's only used when the compiler flag USE_CPP_EXCEPTIONS is set to ON,
 * the default value is off.
 */
class lua_exception : public std::runtime_error
{
  public:
    explicit lua_exception(const std::string& msg);
    explicit lua_exception(const char* msg);
    const char* what() const noexcept override;

  private:
};
}
#endif
