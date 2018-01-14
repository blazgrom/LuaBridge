#ifndef LUABZ_LUA_VAL_EXPR_HPP
#define LUABZ_LUA_VAL_EXPR_HPP
#include "callable_traits.hpp"
#include "detail/lua_error.hpp"
#include "detail/lua_value.hpp"
#include "variadic_index.hpp"
#include <cassert>
#include <cstddef>
#include <functional>
#include <lua.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
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
    static std::vector<std::function<int(lua_State*)>>
        registered_functions;  ///< Contains all C++ registered function that
                               ///< can be called through lua or lua_value_ref
    static std::unordered_map<lua_State*, std::vector<int>>
        file_registered_functions;
    /**
     * \brief RAII helper used to push to the top of the stack a specific
     * lua_variable
     */
    struct lua_var_loader {
        /**
         * Pushes to the top of the stack a specific lua variable
         * \param st The state in which the variable is present
         * \param variable_name The name of the lua variable
         * \param us Starting used stack space
         */
        lua_var_loader(lua_State* st,
                       const std::string& variable_name,
                       int& us);
        /**
         * Pops all the used stack spaces from the associated stack.
         */
        ~lua_var_loader();
        lua_State* state;
        int& used_space;
        int starting_used_space;
    };

  public:
    lua_value_ref(const lua_value_ref& rhs);
    ~lua_value_ref() = default;
    /**
     * \detail The copy c-tor is deleted because: The original was to permit to
     * assign one lua variable to another lua variable through  C++ however,
     * this is not possible because when we have variable from two different
     * states we are forced to pass the data though C++, however in order for
     * that to work any lua type must be representable in C++. AS we know table
     * are one of the types that cannot be represented cleanly in C++. As
     * already said the previous idea doesn't work for table, however it works
     * for when the types of the two variables are something like string or
     * number. Seen that most powerful feature of Lua is it's table i've decided
     * to not give possibility to directly assign one lua variable to another
     * lua variable through lua_value_ref. A useful workaround can be
     * script("variableA=variableB").
     */
    lua_value_ref& operator=(const lua_value_ref& rhs) = delete;
    /**
     * \brief Conversion from lua_value_ref to any type T
     * \pre There must be a specialization of luabz::detail::lua_value with type
     * T in order to be able to perform the conversion
     */
    template <typename T>
    operator T() const
    {
        lua_var_loader loader{m_state, m_name, used_stack_spaces};
        T result = detail::lua_value<T>::get(m_state, -1);
        return result;
    }
    /**
     * \brief Operator for setting the value of the lua variable
     * \note In order to handle global variables and table fields with the same
     * function, we perform an unnecessary load when assigning a value to a
     * global variable. Also when performing an assignment to table's field the
     * last object we load into the stack is the field inself. This means that
     * the table tahat contains the field will always be one position under the
     * field e.g. If our field is at index -1, the table of the field will be at
     * position -2.
     */
    template <typename T>
    lua_value_ref& operator=(const T& new_value)
    {
        lua_var_loader loader{m_state, m_name, used_stack_spaces};
        detail::lua_value<T>::insert(m_state, new_value);
        set_lua_var();
        return *this;
    }
    /**
     *  Registers a C++ lambda that later on can be called from lua code
     * or though lua_value_ref \param user_f The function that you want to be
     * called
     */
    template <typename T>
    void calls(T user_f)
    {
        using namespace Utils;
        registered_functions.emplace_back([
            this, user_function = std::move(user_f)
        ](lua_State*) mutable->int {
            bool correct_number_of_arguments =
                (lua_gettop(m_state) == callable_traits<T>::args_count);
            if (correct_number_of_arguments) {
                std::pair<callable_return_type<T>, callable_arg_types<T>> t{};
                return call_registered_function(user_function, t);
            }
            // 0 Indicates to Lua that the function wasn't invoke and there
            // are no return values
            return 0;
        });
        auto inserted_function_position = registered_functions.size() - 1;
        file_registered_functions[m_state].push_back(
            inserted_function_position);
        register_function(inserted_function_position);
    }
    /**
     * Registers a C++ std::function that later on can be called from lua code
     * or though lua_value_ref \param user_f The function that you want to be
     * called
     */
    template <typename ReturnType, typename... Args>
    void calls(std::function<ReturnType(Args...)> user_f)
    {
        registered_functions.emplace_back(
            [ this, user_function = std::move(user_f) ](lua_State*)->int {
                bool correct_number_of_arguments =
                    (lua_gettop(m_state) == sizeof...(Args));
                if (correct_number_of_arguments) {
                    std::pair<ReturnType, std::tuple<Args...>> t{};
                    return call_registered_function(user_function, t);
                }
                // 0 Indicates to Lua that the function wasn't invoke and there
                // are no return values
                return 0;
            });
        auto inserted_function_position = registered_functions.size() - 1;
        file_registered_functions[m_state].push_back(
            inserted_function_position);
        register_function(inserted_function_position);
    }
    /**
     *  Registers a C++ function pointer that later on can be called from lua
     * code or though lua_value_ref \param user_f The function that you want to
     * be called
     */
    template <typename ReturnType, typename... Args>
    void calls(ReturnType (*user_f)(Args...))
    {
        calls(std::function<ReturnType(Args...)>(user_f));
    }
    /**
     * \brief Calls a registered function, it also extracts function's
     * parameters from the\n lua stack and insert the return value of the
     * function into the stack
     */
    template <typename T, typename ReturnType, typename... Args>
    int call_registered_function(T& user_f,
                                 std::pair<ReturnType, std::tuple<Args...>>&)
    {
        Utils::variadric_index<Args...> index_generator;
        auto result = user_f(detail::lua_value<Args>::get(
            m_state, (index_generator.get_index() + 1) * -1)...);
        detail::lua_value<decltype(result)>::insert(m_state, result);
        return 1;
    }

    /**
     * \brief Calls a lua function or a C++ function assign to lua variable
     * \param args The arguments that are passed to the function
     */
    template <typename... Args>
    lua_value_ref operator()(Args&&... args)
    {
        lua_var_loader loader{m_state, m_name, used_stack_spaces};
        if (!lua_isfunction(m_state, -1)) {
            detail::lua_error("You are trying to call something that is not "
                              "neither Lua function nor C/C++ function");
        }
        const int input_count = sizeof...(Args);
        insert_function_parameters(std::forward<Args>(args)...);
        const int output_count = 1;
        // Note: There is no need to increment used_stack_spaces for
        // the arguments because lua_pcall automatically pops them
        if (lua_pcall(m_state, input_count, output_count, 0) != 0) {
            detail::lua_error(lua_tostring(m_state, -1));
        }
        auto return_value_name = generate_return_value_name();
        lua_setfield(m_state, LUA_GLOBALSINDEX, return_value_name.c_str());
        return lua_value_ref(m_state, return_value_name);
    }
    /**
     * \brief Operator= between lua_value_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T
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
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T
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
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T\n The return type is the same as
     * applying the operator on two variables of type T
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
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T\n The return type is the same as
     * applying the operator on two variables of type T
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
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T\n The return type is the same as
     * applying the operator on two variables of type T
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
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T\n The return type is the same as
     * applying the operator on two variables of type T
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
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T\n The return type is the same as
     * applying the operator on two variables of type T
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
     * \brief Checks if the value identified by the name of lua_value_ref is
     * equal to lua's nil
     */
    bool is_nil() const;
    /**
     * \brief Access a the field identified by name inside the table identified
     * by this lua_value_ref
     */
    lua_value_ref operator[](const std::string& field_name) const;
    lua_value_ref operator[](const char* field_name) const;

  private:
    lua_value_ref(lua_State* state, std::string name);
    /**
     * \brief Determines if m_name identifies a field in lua table
     */
    bool is_table_field() const;
    /**
     * \brief Sets the lua variable to a new value
     * \pre The new value is already be on top of the lua stack
     * (m_state in this case).
     */
    void set_lua_var();
    /**
     * \brief Extracts the name of the field if m_name contains the name field
     * in a table e.g, TableA.Field will return Field
     * */
    std::string get_field_name() const;

    /**
     * Helper function for pushing the varaible associated by the this object
     * and the variable associated with the rhs object and performing a call to
     * a specific lua_operator on the previously loaded values
     */
    bool call_lua_operator(const lua_value_ref& rhs,
                           int (*lua_operator)(lua_State* l,
                                               int index1,
                                               int index2)) const;
    /**
     * Generated a random name composed by
     * m_name + "_" + random number generated by stl's default random engine and
     * uniform int distribution \note This done in order to eliminate the
     * following scenario.\n Let's say the registered function 'something' just
     * return the argument from the input, if we use the same name the
     * lua_value_ref object 'a' and 'b' will both have a value of 2 at the end
     * of the executuion of the second line of code.
     * a=script["something"](1);
     * b=script["something"](2);
     */
    std::string generate_return_value_name() const;
    /**
     * Insert the function parameters when we are invoking a function though a
     * lua_value_ref \param value The first argument of the function \param args
     * All the other parameters
     */
    template <typename T, typename... Args>
    void insert_function_parameters(T&& value, Args&&... args)
    {
        detail::lua_value<T>::insert(m_state, value);
        insert_function_parameters(std::forward<Args>(args)...);
    }
    /**
     * \brief "Specialization" for when we want to insert one function parameter
     */
    template <typename T>
    void insert_function_parameters(T&& value)
    {
        detail::lua_value<T>::insert(m_state, value);
    }
    /**
     * \brief "Specialization" for when we want to insert no function
     * paramaters, this function simply returns \sa insert_prameters(T&& value,
     * Args&&... args)
     */
    void insert_function_parameters() { return; }
    /**
     * Assigns lua_CFunction to the variable identified by m_name.
     * This lua_CFunctions calls the function at index function_position_index
     * inside registered_functions when called. \param function_position_index
     * the position of the function you want to call inside registered_functions
     */
    void register_function(std::size_t function_position_index);
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