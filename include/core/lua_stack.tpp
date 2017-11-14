#ifndef LUA_STACK_TEMPLATE_IMPL
    #error This file should be included only from lua_stack.hpp
#endif
//Explicit specializations
template<>
inline lua_table lua_stack::get<lua_table>(int index) const
{
    if (!lua_istable(m_state, index))
    {
        throw lua_error("The type you are trying to retrieve cannot be constructed with a lua_table");
    }
    return  create_lua_table();
}
template<>
inline std::string lua_stack::get<std::string>(int index) const
{
    size_t strLength = 0;
    const char* str = lua_tolstring(m_state, index, &strLength);
    std::string r{ str, strLength };
    return r;
}
template <>
inline char lua_stack::get<char>(int index) const
{
    auto str = get<std::string>(index);
    return str[0];
}
template <>
inline bool lua_stack::get<bool>(int index) const
{
    return lua_toboolean(m_state, index) != 0;
}
template <>
inline short lua_stack::get<short>(int index) const
{
    return get_lua_integer<short>(index, Utils::Can_represent_value<lua_Integer, short>{});
}
template <>
inline unsigned short lua_stack::get<unsigned short>(int index) const
{
    return get_lua_integer<unsigned short>(index, Utils::Can_represent_value<lua_Integer, unsigned short>{});
}
template <>
inline int lua_stack::get<int>(int index) const
{
    return get_lua_integer<int>(index, Utils::Can_represent_value<lua_Integer, int>{});
}
template <>
inline unsigned int lua_stack::get<unsigned int>(int index) const
{
    return get_lua_integer<unsigned int>(index, Utils::Can_represent_value<lua_Integer, unsigned int>{});
}
template <>
inline long lua_stack::get<long>(int index) const
{
    return get_lua_integer<long>(index, Utils::Can_represent_value<lua_Integer, long>{});
}
template <>
inline unsigned long lua_stack::get<unsigned long>(int index) const
{
    return get_lua_integer<unsigned long>(index, Utils::Can_represent_value<lua_Integer, unsigned long>{});
}
template <>
inline long long lua_stack::get<long long>(int index) const
{
    return get_lua_integer<long long>(index, Utils::Can_represent_value<lua_Integer, long long>{});
}
template <>
inline unsigned long long lua_stack::get<unsigned long long>(int index) const
{
    return get_lua_integer<unsigned long long>(index, Utils::Can_represent_value<lua_Integer, unsigned long long>{});
}
template <>
inline double  lua_stack::get<double>(int index) const
{
    return get_lua_number<double>(index, Utils::Can_represent_value<lua_Number, double>{});
}
template <>
inline float  lua_stack::get<float>(int index) const
{
    return get_lua_number<float>(index, Utils::Can_represent_value<lua_Number, float>{});
}