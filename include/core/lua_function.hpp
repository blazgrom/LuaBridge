#ifndef LUABZ_LUA_FUNCTION_HPP
#define LUABZ_LUA_FUNCTION_HPP
#include <string>
namespace LuaBz
{
template <typename... T>
class lua_function
{
  public:
    explicit lua_function(const std::string &name);
    const std::string &name();
    std::string name() const;
    unsigned int result_count() const;

  private:
    std::string m_name;
    const unsigned int m_resultCount;
};
#ifndef LUA_FUNCTION_TEMPLATE_IMPL
#define LUA_FUNCTION_TEMPLATE_IMPL
#include "lua_function.tpp"
#undef LUA_FUNCTION_TEMPLATE_IMPL
#endif
}
#endif  // !LUABZ_LUA_FUNCTION_HPP
