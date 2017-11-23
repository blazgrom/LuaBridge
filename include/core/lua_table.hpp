#ifndef LUABZ_LUA_TABLE_HPP
#define LUABZ_LUA_TABLE_HPP
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "lua_type.hpp"
#include "various.hpp"
namespace LuaBz
{
class lua_table
{
  public:
    class lua_value;

  public:
    lua_table();
    lua_table(typename std::vector<lua_value>::size_type count,
              const lua_value &value);
    explicit lua_table(typename std::vector<lua_value>::size_type count);
    template <class Iter>
    lua_table(Iter first, Iter last);
    lua_table(const std::initializer_list<lua_value> &init);
    lua_table(const lua_table &rhs) = default;
    lua_table(lua_table &&rhs) = default;
    lua_table &operator=(std::initializer_list<lua_value> ilist);
    lua_table &operator=(const lua_table &rhs) = default;
    lua_table &operator=(lua_table &&rhs) = default;
    ~lua_table() = default;
    typename std::vector<lua_value>::reference at(
        typename std::vector<lua_value>::size_type pos);
    typename std::vector<lua_value>::const_reference at(
        typename std::vector<lua_value>::size_type pos) const;
    typename std::vector<lua_value>::reference operator[](
        typename std::vector<lua_value>::size_type pos);
    typename std::vector<lua_value>::const_reference operator[](
        typename std::vector<lua_value>::size_type pos) const;

    typename std::vector<lua_value>::reference operator[](
        const std::string &name);
    typename std::vector<lua_value>::const_reference operator[](
        const std::string &name) const;
    typename std::vector<lua_value>::reference front();
    typename std::vector<lua_value>::const_reference front() const;
    typename std::vector<lua_value>::reference back();
    typename std::vector<lua_value>::const_reference back() const;
    typename std::vector<lua_value>::iterator begin() noexcept;
    typename std::vector<lua_value>::const_iterator begin() const noexcept;
    typename std::vector<lua_value>::const_iterator cbegin() const noexcept;
    typename std::vector<lua_value>::iterator end() noexcept;
    typename std::vector<lua_value>::const_iterator end() const noexcept;
    typename std::vector<lua_value>::const_iterator cend() const noexcept;
    typename std::vector<lua_value>::reverse_iterator rbegin() noexcept;
    typename std::vector<lua_value>::const_reverse_iterator rbegin() const
        noexcept;
    typename std::vector<lua_value>::const_reverse_iterator crbegin() const
        noexcept;
    typename std::vector<lua_value>::reverse_iterator rend() noexcept;
    typename std::vector<lua_value>::const_reverse_iterator rend() const
        noexcept;
    typename std::vector<lua_value>::const_reverse_iterator crend() const
        noexcept;
    bool empty() const noexcept;
    typename std::vector<lua_value>::size_type size() const noexcept;
    void clear() noexcept;
    void push_back(const lua_value &val);
    void push_back(lua_value &&val);
    void pop_back();
    template <class... Args>
    void emplace_back(Args &&... args);
    void swap(lua_table &other);
    friend bool operator==(const lua_table &lhs, const lua_table &rhs);
    friend bool operator!=(const lua_table &lhs, const lua_table &rhs);
    friend bool operator<(const lua_table &lhs, const lua_table &rhs);
    friend bool operator<=(const lua_table &lhs, const lua_table &rhs);
    friend bool operator>(const lua_table &lhs, const lua_table &rhs);
    friend bool operator>=(const lua_table &lhs, const lua_table &rhs);

  private:
    std::vector<lua_value> m_values;
};

class lua_table::lua_value
{
  public:
    lua_value();
    lua_value(const std::string &name, lua_t::nil data);
    lua_value(const std::string &name, lua_t::integer data);
    lua_value(const std::string &name, lua_t::number data);
    lua_value(const std::string &name, lua_t::boolean data);
    lua_value(const std::string &name, const char *data);
    lua_value(const std::string &name, const lua_t::string &data);
    lua_value(const std::string &name, const lua_table &data);
    lua_value(const lua_value &rhs);
    lua_value(lua_value &&rhs);
    lua_value &operator=(const lua_value &rhs);
    lua_value &operator=(lua_value &&rhs);
    lua_value &operator=(lua_t::integer rhs);
    lua_value &operator=(lua_t::number rhs);
    lua_value &operator=(lua_t::boolean rhs);
    lua_value &operator=(lua_t::nil rhs);
    lua_value &operator=(const lua_t::string &rhs);
    lua_value &operator=(const char *rhs);
    lua_value &operator=(const lua_table &rhs);
    operator lua_t::integer() const;
    operator lua_t::number() const;
    explicit operator lua_t::boolean() const;  // This is explicit because it
                                               // hijacks the conversion to
                                               // integer
    operator lua_t::nil() const;
    operator lua_t::string() const;
    operator lua_table() const;
    ~lua_value();
    const std::string &name() const;
    lua_types type() const;
    template <typename T>
    const T &get() const;
    template <typename T>
    T &get();

  private:
    lua_types m_type;
    std::string m_name;
    union {
        lua_t::nil m_nil;
        lua_t::integer m_integer;
        lua_t::number m_number;
        lua_t::boolean m_bool;
        lua_t::string m_string;
        lua_table m_table;
    };
    void init(const lua_value &rhs);
    void copy(const lua_value &rhs);
    void destroy_complex();
};
bool operator<(const lua_table::lua_value &lhs,
               const lua_table::lua_value &rhs);
bool operator==(const lua_table::lua_value &lhs,
                const lua_table::lua_value &rhs);
using lua_value = lua_table::lua_value;
#ifndef LUA_TABLE_TEMPLATE_IMPL
#define LUA_TABLE_TEMPLATE_IMPL
#include "lua_table.tpp"
#undef LUA_TABLE_TEMPLATE_IMPL
#endif
}
#endif  // ! LUABZ_LUA_TABLE_HPP