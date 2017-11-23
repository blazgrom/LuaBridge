#ifndef LUA_TABLE_TEMPLATE_IMPL
#error This file should be included only from lua_table.hpp
#endif
// LuaTable
template <class Iter>
lua_table::lua_table(Iter first, Iter last) : m_values(first, last)
{
}
template <class... Args>
void lua_table::emplace_back(Args &&... args)
{
    m_values.emplace_back(std::forward<Args>(args)...);
}
// LuaValue
template <typename T>
const T &lua_table::lua_value::get() const
{
    static_assert(Utils::always_false<T>::value, "Use of a type not supported "
                                                 "by lua_value");
}
template <typename T>
T &lua_table::lua_value::get()
{
    static_assert(Utils::always_false<T>::value, "Use of a type not supported "
                                                 "by lua_value");
}
// Explicit specializations
template <>
inline const lua_t::integer &lua_table::lua_value::get<lua_t::integer>() const
{
    assert(m_type == lua_types::Integer);
    return m_integer;
}
template <>
inline const lua_t::number &lua_table::lua_value::get<lua_t::number>() const
{
    assert(m_type == lua_types::Number);
    return m_number;
}
template <>
inline const lua_t::boolean &lua_table::lua_value::get<lua_t::boolean>() const
{
    assert(m_type == lua_types::Boolean);
    return m_bool;
}
template <>
inline const lua_t::string &lua_table::lua_value::get<lua_t::string>() const
{
    assert(m_type == lua_types::String);
    return m_string;
}
template <>
inline const lua_t::nil &lua_table::lua_value::get<lua_t::nil>() const
{
    assert(m_type == lua_types::Nil);
    return m_nil;
}
template <>
inline const lua_table &lua_table::lua_value::get<lua_table>() const
{
    assert(m_type == lua_types::Table);
    return m_table;
}
template <>
inline lua_t::integer &lua_table::lua_value::get<lua_t::integer>()
{
    return const_cast<lua_t::integer &>(
        static_cast<const lua_table::lua_value &>(*this).get<lua_t::integer>());
}
template <>
inline lua_t::number &lua_table::lua_value::get<lua_t::number>()
{
    return const_cast<lua_t::number &>(
        static_cast<const lua_table::lua_value &>(*this).get<lua_t::number>());
}
template <>
inline lua_t::boolean &lua_table::lua_value::get<lua_t::boolean>()
{
    return const_cast<lua_t::boolean &>(
        static_cast<const lua_table::lua_value &>(*this).get<lua_t::boolean>());
}
template <>
inline lua_t::string &lua_table::lua_value::get<lua_t::string>()
{
    return const_cast<lua_t::string &>(
        static_cast<const lua_table::lua_value &>(*this).get<lua_t::string>());
}
template <>
inline lua_t::nil &lua_table::lua_value::get<lua_t::nil>()
{
    return const_cast<lua_t::nil &>(
        static_cast<const lua_table::lua_value &>(*this).get<lua_t::nil>());
}
template <>
inline lua_table &lua_table::lua_value::get<lua_table>()
{
    return const_cast<lua_table &>(
        static_cast<const lua_table::lua_value &>(*this).get<lua_table>());
}