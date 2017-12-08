#ifndef LUABZ_LUA_VAL_EXPR_HPP
#define LUABZ_LUA_VAL_EXPR_HPP
#include <cstddef>
#include <lua.hpp>
#include <string>
#include "detail/lua_value.hpp"
namespace luabz
{
class lua_script;

class lua_value_ref
{
    friend class luabz::lua_script;

  public:
    lua_value_ref(const lua_value_ref& rhs);
    ~lua_value_ref() = default;
    lua_value_ref& operator=(const lua_value_ref& rhs);
    template <typename T>
    operator T() const
    {
        load_lua_var();
        T result = detail::lua_value<T>::get(m_state);
        // clear_used_stack_spaces();
        return result;
    }
    template <typename T>
    lua_value_ref& operator=(const T& new_value)
    {
        detail::lua_value<T>::set(m_state, m_name, new_value);
        return *this;
    }
    template <typename T>
    bool operator==(const T& rhs) const
    {
        T lhs = *this;  // Implicit cast to type T
        return lhs == rhs;
    }
    bool operator==(const lua_value_ref& rhs) const;
    /**
     * \note this is converted to type T
     * \todo Decide if the conversion to type T is correct in all cases
     */
    template <typename T>
    bool operator<(const T& rhs) const
    {
        T lhs = *this;  // Implicit cast to type T
        return lhs < rhs;
    }
    bool operator<(const lua_value_ref& rhs) const;

    template <typename T>
    bool operator!=(const T& rhs) const
    {
        return !(*this == rhs);
    }
    /**
     * \note this is converted to type T
     * \todo Decide if the conversion to type T is correct in all cases
     */
    template <typename T>
    bool operator>(const T& rhs) const
    {
        T lhs = *this;  // Implicit cast to type T
        return rhs < lhs;
    }
    template <typename T>
    bool operator<=(const T& rhs) const
    {
        return !(this->operator>(rhs));
    }
    template <typename T>
    bool operator>=(const T& rhs) const
    {
        return !(this->operator<(rhs));
    }
    /**
     * \note this is converted to type T
     * \todo Decide if the conversion to type T is correct in all cases
     */
    template <typename T>
    auto operator+(const T& rhs) const -> decltype(rhs + rhs)
    {
        T lhs = *this;
        return lhs + rhs;
    }
    template <typename T>
    lua_value_ref& operator+=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator+(rhs)));
        return *this;
    }
    /**
     * \note this is converted to type T
     * \todo Decide if the conversion to type T is correct in all cases
     */
    template <typename T>
    auto operator-(const T& rhs) const -> decltype(rhs - rhs)
    {
        T lhs = *this;
        return lhs - rhs;
    }

    template <typename T>
    lua_value_ref& operator-=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator-(rhs)));
        return *this;
    }
    /**
     * \note this is converted to type T
     * \todo Decide if the conversion to type T is correct in all cases
     */
    template <typename T>
    auto operator/(const T& rhs) const -> decltype(rhs / rhs)
    {
        T lhs = *this;
        return lhs / rhs;
    }

    template <typename T>
    lua_value_ref& operator/=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator/(rhs)));
        return *this;
    }
    /**
     * \note this is converted to type T
     * \todo Decide if the conversion to type T is correct in all cases
     */
    template <typename T>
    auto operator*(const T& rhs) const -> decltype(rhs * rhs)
    {
        T lhs = *this;
        return lhs * rhs;
    }

    template <typename T>
    lua_value_ref& operator*=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator*(rhs)));
        return *this;
    }
    void operator+(const lua_value_ref& rhs) = delete;
    void operator+=(const lua_value_ref& rhs) = delete;
    void operator/(const lua_value_ref& rhs) = delete;
    void operator/=(const lua_value_ref& rhs) = delete;
    void operator-(const lua_value_ref& rhs) = delete;
    void operator-=(const lua_value_ref& rhs) = delete;
    void operator*(const lua_value_ref& rhs) = delete;
    void operator*=(const lua_value_ref& rhs) = delete;
    /**
     * Checks if the value identified by the name of lua_value_ref is equal to
     * lua's nil
     */
    bool is_nil() const;

    /**
     * Access a the field identified by name inside the table identified by this
     * lua_value_ref
     */
    lua_value_ref operator[](const std::string& name) const;

  private:
    lua_value_ref(lua_State* state, const std::string& name);
    /**
     * determines if the variable that the this object represents is a lua table
     * field
     */
    bool is_table_field() const;
    /**
     * \brief Pushes to the top of the stack the lua variable identified by
     * m_name
     */
    void load_lua_var() const;
    /**
     * \brief Sets the lua variable to a new value
     * \pre The new value is already be on top of the lua stack
     * (m_state in this case).
     */
    void set_lua_var();
    /**
     * Removes X element from the lua stack, where X is identified by
     * used_stack_spaces;
     */
    void clear_used_stack_spaces() const;
    lua_State* m_state;  ///< A lua state which represents the file with which
                         ///< the script was opened
    const std::string
        m_name;            ///< The name of the lua variable which the object
                           ///< represents
    static const int top;  ///< A simple identifier for the top element of the
                           ///< stack
    static const char
        lua_table_field_delimeter;  ///< Idenfies the delimeter
                                    ///< used to distinguish when
                                    ///< the object is pointing to
                                    ///< a simple lua variable and
                                    ///< when it's point to a field
                                    ///< inside lua table
    mutable int used_stack_spaces;  ///< How many elements are occupied
                                    ///< on the lua stack currectly,
                                    ///< this variable is incremented
                                    ///< every time we insert a new
                                    ///< element on the stack and is
                                    ///< zeroed out when we call the
                                    ///< member function
                                    ///< clear_used_stack_spaces, which
                                    ///< pops the elements from the
                                    ///< stack and sets the variable to
                                    ///< zero. The variable should be
                                    ///< mutable because, we want to be
                                    ///< able to clear the stack if
                                    ///< necessary even when accessing
                                    ///< fields on a const object
};
}
#endif