#ifndef LUA_TABLE_TEMPLATE_IMPL
    #error This file should be included only from lua_table.hpp
#endif
//LuaTable
template <class Iter>
lua_table::lua_table(Iter first, Iter last)
    :
    m_values(first, last)
{

}
template <class...Args>
void lua_table::emplace_back(Args&&... args)
{
    m_values.emplace_back(std::forward<Args>(args)...);
}
//LuaValue
template <typename T>
T lua_table::lua_value::value() const
{
    static_assert(Utils::always_false<T>::value, "Use of a type not supported by lua_value");
}
template <typename T>
const T& lua_table::lua_value::value()
{
    static_assert(Utils::always_false<T>::value, "Use of a type not supported by lua_value");
}
//Explicit specializations
template <>
inline int lua_table::lua_value::value<int>() const
{
    return integer();
}
template <>
inline double lua_table::lua_value::value<double>() const
{
    return number();
}
template <>
inline bool lua_table::lua_value::value<bool>() const
{
    return boolean();
}
template <>
inline std::string lua_table::lua_value::value<std::string>() const
{
    return string();
}
template <>
inline std::nullptr_t lua_table::lua_value::value<std::nullptr_t>() const
{
    return nil();
}
template<>
inline lua_table lua_table::lua_value::value<lua_table>() const
{
    return table();
}
template <>
inline const int& lua_table::lua_value::value<int>()
{
    return m_integer;
}
template <>
inline const double& lua_table::lua_value::value<double>()
{
    return m_number;
}
template <>
inline const bool& lua_table::lua_value::value<bool>()
{
    return m_bool;
}
template <>
inline const std::string& lua_table::lua_value::value<std::string>()
{
    return string();
}
template <>
inline const std::nullptr_t& lua_table::lua_value::value<std::nullptr_t>()
{
    return m_nil;
}
template<>
inline const lua_table& lua_table::lua_value::value<lua_table>()
{
    return table();
}