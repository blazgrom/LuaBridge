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
T lua_table::lua_value::get() const
{
    static_assert(Utils::always_false<T>::value, "Use of a type not supported by lua_value");
}
template <typename T>
const T& lua_table::lua_value::get()
{
    static_assert(Utils::always_false<T>::value, "Use of a type not supported by lua_value");
}
//Explicit specializations
template <>
inline lua_t::integer lua_table::lua_value::get<lua_t::integer>() const
{
    return get_integer();
}
template <>
inline lua_t::number lua_table::lua_value::get<lua_t::number>() const
{
    return get_number();
}
template <>
inline lua_t::boolean lua_table::lua_value::get<lua_t::boolean>() const
{
    return get_boolean();
}
template <>
inline lua_t::string lua_table::lua_value::get<lua_t::string>() const
{
    return get_string();
}
template <>
inline lua_t::nil lua_table::lua_value::get<lua_t::nil>() const
{
    return get_nil();
}
template<>
inline lua_table lua_table::lua_value::get<lua_table>() const
{
    return get_table();
}
template <>
inline const lua_t::integer& lua_table::lua_value::get<lua_t::integer>()
{
    return get_integer();
}
template <>
inline const lua_t::number& lua_table::lua_value::get<lua_t::number>()
{
    return get_number();
}
template <>
inline const lua_t::boolean& lua_table::lua_value::get<lua_t::boolean>()
{
    return get_boolean();
}
template <>
inline const lua_t::string& lua_table::lua_value::get<lua_t::string>()
{
    return get_string();
}
template <>
inline const lua_t::nil& lua_table::lua_value::get<lua_t::nil>()
{
    return get_nil();
}
template<>
inline const lua_table& lua_table::lua_value::get<lua_table>()
{
    return get_table();
}