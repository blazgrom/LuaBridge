#ifndef LUABZ_LUA_VAL_EXPR_HPP
#define LUABZ_LUA_VAL_EXPR_HPP
#include <cstddef>
#include <lua.hpp>
#include <string>
#include "detail/lua_value.hpp"
namespace luabz
{
class lua_script;
/**
 * \brief C++ representation of Lua variable
 */
class lua_value_ref
{
    friend class luabz::lua_script;  ///< The lua_script class it's a friend
                                     ///< because we want the only way to create
                                     ///< lua_value_ref to be through a
                                     ///< lua_script

  public:
    lua_value_ref(const lua_value_ref& rhs);
    ~lua_value_ref() = default;
    lua_value_ref& operator=(const lua_value_ref& rhs);
    /**
     * \brief Conversion from lua_value_ref to any type T
     * \pre There must be a specialization of luabz::detail::lua_value with type T in order to
     * be able to perform the conversion
    */
    template <typename T>
    operator T() const
    {
        load_lua_var();
        T result = detail::lua_value<T>::get(m_state);
        clear_used_stack_spaces();
        return result;
    }
    /**
     * \brief Operator for setting the value of the lua variable
     * \todo Find a better way of doing the set of a variable, right now we have
     * to load the variable even if the variable is global. Another thing that
     * should be corrected is the case of when we are setting the value of
     * field, seen that we use load_lua_var it pushed all the element on the
     * path to the variable, onto the lua stack, however the last push is not
     * necessary because we are pushing the field while the field is not needed
     * in order to perform set put only to perform get
     */
    template <typename T>
    lua_value_ref& operator=(const T& new_value)
    {
        load_lua_var();
        detail::lua_value<T>::insert(m_state, new_value);
        set_lua_var();
        clear_used_stack_spaces();
        return *this;
    }
    /**
     * \brief Operator= between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents the value of this
     * converted to type T
    */
    template <typename T>
    bool operator==(const T& rhs) const
    {
        T lhs = *this;
        return lhs == rhs;
    }
    /**
     * \brief Operator= between lua_value_ref and lua_value_ref
    */
    bool operator==(const lua_value_ref& rhs) const;
    /**
     * \brief Operator< between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents the value of this
     * converted to type T
     */
    template <typename T>
    bool operator<(const T& rhs) const
    {
        T lhs = *this;  // Implicit cast to type T
        return lhs < rhs;
    }
    /**
     * \brief Operator< between lua_value_ref and lua_value_ref
    */
    bool operator<(const lua_value_ref& rhs) const;
    /**
     * \brief Inequality operator between lua_value_ref and any type T
     * \note Implemented in terms of operator==
    */
    template <typename T>
    bool operator!=(const T& rhs) const
    {
        return !(*this == rhs);
    }
    /**
     * \brief Operator> between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents the value of this
     * converted to type T\n
     * The return type is the same as applying the operator on two variables of type T
    */
    template <typename T>
    bool operator>(const T& rhs) const
    {
        T lhs = *this;  // Implicit cast to type T
        return rhs < lhs;
    }
    /**
     * \brief Operator<= between lua_value_ref and any type T
     * \note Implemented in terms of operator>
    */
    template <typename T>
    bool operator<=(const T& rhs) const
    {
        return !(this->operator>(rhs));
    }
    /**
     * \brief Operator>= between lua_value_ref and any type T
     * \note Implemented in terms of operator<
    */
    template <typename T>
    bool operator>=(const T& rhs) const
    {
        return !(this->operator<(rhs));
    }
   /**
     * \brief Operator+ between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents the value of this
     * converted to type T\n
     * The return type is the same as applying the operator on two variables of type T
    */
    template <typename T>
    auto operator+(const T& rhs) const -> decltype(rhs + rhs)
    {
        T lhs = *this;
        return lhs + rhs;
    }
    /**
     * \brief Operator for performing += between lua_value_ref and any type T
     */
    template <typename T>
    lua_value_ref& operator+=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator+(rhs)));
        return *this;
    }
    /**
     * \brief Operator- between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents the value of this
     * converted to type T\n
     * The return type is the same as applying the operator on two variables of type T
    */
    template <typename T>
    auto operator-(const T& rhs) const -> decltype(rhs - rhs)
    {
        T lhs = *this;
        return lhs - rhs;
    }
    /**
     * \brief Operator for performing -= between lua_value_ref and any type T
     */
    template <typename T>
    lua_value_ref& operator-=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator-(rhs)));
        return *this;
    }
    /**
     * \brief Operator/ between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents the value of this
     * converted to type T\n
     * The return type is the same as applying the operator on two variables of type T
    */
    template <typename T>
    auto operator/(const T& rhs) const -> decltype(rhs / rhs)
    {
        T lhs = *this;
        return lhs / rhs;
    }
    /**
     * \brief Operator for performing /= between lua_value_ref and any type T
     */
    template <typename T>
    lua_value_ref& operator/=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator/(rhs)));
        return *this;
    }
   /**
     * \brief Operator* between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents the value of this
     * converted to type T\n
     * The return type is the same as applying the operator on two variables of type T
    */
    template <typename T>
    auto operator*(const T& rhs) const -> decltype(rhs * rhs)
    {
        T lhs = *this;
        return lhs * rhs;
    }
    /**
     * \brief Operator for performing *= between lua_value_ref and any type T
     */
    template <typename T>
    lua_value_ref& operator*=(const T& rhs)
    {
        this->operator=(static_cast<T>(this->operator*(rhs)));
        return *this;
    }
    /**
     * This operator it's deleted because it's now clear what exatcly should
     * happen when performing this operator on two lua_value_ref, but why is it
     * unclear what should happen. Because the lua C api doesn't provide
     * function like add/division/minus/multiplication so in order to perform
     * those operation we have first to convert one of the lua_value_ref
     * variables into a C++ variable, but we cannot do it because we cannot
     * decide for the user the type of a variable. \note We can always perform
     * this operation in the following way \n type var=script["var"];
     * script["var"]+var;
     */
    void operator+(const lua_value_ref& rhs) = delete;
    /**
     * \sa operator+()
     */
    void operator+=(const lua_value_ref& rhs) = delete;
    /**
     * \sa operator+()
     */
    void operator/(const lua_value_ref& rhs) = delete;
    /**
     * \sa operator+()
     */
    void operator/=(const lua_value_ref& rhs) = delete;
    /**
     * \sa operator+()
     */
    void operator-(const lua_value_ref& rhs) = delete;
    /**
     * \sa operator+()
     */
    void operator-=(const lua_value_ref& rhs) = delete;
    /**
     * \sa operator+()
     */
    void operator*(const lua_value_ref& rhs) = delete;
    /**
     * \sa operator+()
     */
    void operator*=(const lua_value_ref& rhs) = delete;
    /**
     * \brief Checks if the value identified by the name of lua_value_ref is equal to
     * lua's nil
     */
    bool is_nil() const;
    /**
     * \brief Access a the field identified by name inside the table identified by this
     * lua_value_ref
     */
    lua_value_ref operator[](const std::string& field_name) const;
  private:
    lua_value_ref(lua_State* state, std::string name);
    /**
     * \brief Determines if m_name identifies a field in lua table
     */
    bool is_table_field() const;
    /**
     * \brief Pushes to the top of the stack the lua variable identified by
     * m_name
     * \sa m_name
     */
    void load_lua_var() const;
    /**
     * \brief Sets the lua variable to a new value
     * \pre The new value is already be on top of the lua stack
     * (m_state in this case).
     */
    void set_lua_var();
    /**
     * \brief Removes X element from the lua stack, where X is identified by
     * used_stack_spaces;
     * \sa used_stack_spaces
     */
    void clear_used_stack_spaces() const;
    /**
     * \brief Extracts the name of the field if m_name contains the name field in a table e.g, TableA.Field will return Field
     * */
    std::string get_field_name() const;
    lua_State* m_state;  ///< A lua state which represents the file with which
                         ///< the script was opened
    const std::string m_name;  ///< The name of the lua variable which the
                               ///< object represents, if the name is that of a
                               ///< table's field it contains it's full name e.g
                               ///< TableA.TableB.TableC.fieldName
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
}  // namespace luabz
#endif