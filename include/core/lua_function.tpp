#ifndef LUA_FUNCTION_TEMPLATE_IMPL
#error This file should only be included from lua_function.hpp
#endif
template <typename... T>
lua_function<T...>::lua_function(const std::string &name)
  : m_name(name), m_resultCount(sizeof...(T))
{
}
template <typename... T>
const std::string &lua_function<T...>::name()
{
    return m_name;
}
template <typename... T>
std::string lua_function<T...>::name() const
{
    return m_name;
}
template <typename... T>
unsigned int lua_function<T...>::result_count() const
{
    return m_resultCount;
}
template <>
inline lua_function<void>::lua_function(const std::string &name)
  : m_name(name), m_resultCount(0)
{
}