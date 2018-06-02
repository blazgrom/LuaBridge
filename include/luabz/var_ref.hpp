#pragma once
#include "error.hpp"
#include "traits/callable_traits.hpp"
#include "value.hpp"
#include "var_loader.hpp"
#include <chrono>
#include <cstddef>
#include <functional>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
namespace luabz
{
class script;
/**
 * \brief C++ representation of Lua variable
 */
class var_ref
{
    friend class luabz::script;

    lua_State* m_state;

    const std::string m_name;

  public:
    var_ref(const var_ref& rhs) : m_state{rhs.m_state}, m_name{rhs.m_name} {}

    ~var_ref() = default;

    bool is_nil() const
    {
        var_loader loader(m_state, m_name);
        auto result = static_cast<bool>(lua_isnil(m_state, -1));
        return result;
    }

    /**
     *  Registers a C++ lambda that later on can be called from lua code
     * or though var_ref \param user_f The function that you want to be
     * called
     */
    template <typename T>
    void assign(T user_f);

    template <typename Class, typename ReturnType, typename... Args>
    void assign(Class* obj, ReturnType (Class::*member)(Args...));

    /**
     * \sa assign(T)
     */
    template <typename ReturnType, typename... Args>
    void assign(std::function<ReturnType(Args...)> user_f);

    /**
     * \sa assign(T)
     */
    template <typename ReturnType, typename... Args>
    void assign(ReturnType (*user_f)(Args...));

    template <typename... Args>
    var_ref operator()(Args&&... args);

    template <typename... Results, typename... Args>
    std::tuple<Results...> call(Args&&...);
    /**
     * \brief Conversion from var_ref to any type T
     * \pre There must be a specialization of luabz::value with type
     * T in order to be able to perform the conversion
     */
    template <typename T>
    operator T() const;

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
    var_ref& operator=(T value);

    /**
     * \brief Operator= between var_ref and any type T
     * \note Temporary variable of type T is created, this temporal represents
     * the value of this converted to type T
     */
    template <typename T>
    bool operator==(const T& rhs) const;

    /**
     * \sa operator==(const T& rhs)
     */
    template <typename T>
    bool operator<(const T& rhs) const;

    template <typename T>
    bool operator!=(const T& rhs) const;
    template <typename T>
    bool operator>(const T& rhs) const;

    template <typename T>
    bool operator<=(const T& rhs) const;

    template <typename T>
    bool operator>=(const T& rhs) const;

    template <typename T>
    auto operator+(const T& rhs) const -> decltype(rhs + rhs);

    template <typename T>
    var_ref& operator+=(const T& rhs);

    template <typename T>
    auto operator-(const T& rhs) const -> decltype(rhs - rhs);

    template <typename T>
    var_ref& operator-=(const T& rhs);

    template <typename T>
    auto operator/(const T& rhs) const -> decltype(rhs / rhs);

    template <typename T>
    var_ref& operator/=(const T& rhs);

    template <typename T>
    auto operator*(const T& rhs) const -> decltype(rhs * rhs);

    template <typename T>
    var_ref& operator*=(const T& rhs);

    bool operator<(const var_ref& rhs) const { return call_lua_operator(rhs, lua_lessthan); }

    bool operator==(const var_ref& rhs) const { return call_lua_operator(rhs, lua_equal); }

    /**
     * \brief Access a field of the current var_ref
     */
    var_ref operator[](const std::string& field_name) const
    {
        var_loader loader(m_state, m_name);
        if (!lua_istable(m_state, -1)) {
            luabz::error("What you are trying to access is not a lua "
                         "table so it cannot be accessed as a table");
        }
        std::string full_name = m_name + '.' + field_name;
        var_ref result(m_state, full_name);
        return result;
    }

  private:
    var_ref(lua_State* state, std::string name) : m_state{state}, m_name{std::move(name)} {}

    bool is_table_field() const { return m_name.find('.') != std::string::npos; }

    std::string get_field_name() const { return m_name.substr(m_name.find_last_of('.') + 1); }

    void set_lua_var()
    {
        static const int object_table_index = -3;
        int lua_table_index = (is_table_field()) ? object_table_index : LUA_GLOBALSINDEX;
        std::string lua_variable_name = (is_table_field()) ? get_field_name() : m_name;
        lua_setfield(m_state, lua_table_index, lua_variable_name.c_str());
    }

    void insert_CFunction(std::function<int(lua_State*)>&& user_function)
    {
        std::size_t user_function_index =
            state::register_cpp_function(m_state, std::move(user_function));
        var_loader loader(m_state, m_name);
        value<decltype(user_function_index)>::insert(m_state, user_function_index);
        lua_CFunction new_value = [](lua_State* functionState) -> int {
            int functionIndex =
                lua_tointeger(functionState, lua_upvalueindex(1));  // retrieve  upvalue
            auto function = state::get_cpp_registered_function(functionIndex);
            return function(functionState);
        };
        value<lua_CFunction>::insert(m_state, new_value, 1);
        set_lua_var();
    }

    bool call_lua_operator(const var_ref& rhs,
                           int (*lua_operator)(lua_State* l, int index1, int index2)) const
    {
        int lhs_index = -2, rhs_index = -1;
        var_loader lhs_loader(m_state, m_name);
        var_loader rhs_loader(rhs.m_state, rhs.m_name);
        if (m_state != rhs.m_state) {
            lua_xmove(rhs.m_state, m_state, 1);
            // TODO:Find better way of doing this
            lhs_loader.increment_used_space();
        }
        auto result = static_cast<bool>(lua_operator(m_state, lhs_index, rhs_index));
        return result;
    }

    std::string generate_return_value_name() const
    {
        std::uniform_int_distribution<int> distribution;
        auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::default_random_engine dre(seed);
        std::string generated_name = m_name + "_";
        generated_name += std::to_string(distribution(dre));
        return generated_name;
    }

    /**
     * \brief assign a registered function, it also extracts function's
     * parameters from the lua stack and insert the return value of the
     * function into the stack
     */
    template <typename T, typename ReturnType, typename... Args, std::size_t... I>
    int call_registered_function(T& user_f,
                                 std::pair<ReturnType, std::tuple<Args...>>&,
                                 std::index_sequence<I...>&&);

    /**
     * \brief assign a registered function, it also extracts function's
     * parameters from the lua stack and insert the return value of the
     * function into the stack
     */
    template <typename C, typename F, typename ReturnType, typename... Args, std::size_t... I>
    int call_registered_function(C obj,
                                 F user_f,
                                 std::pair<ReturnType, std::tuple<Args...>>&,
                                 std::index_sequence<I...>&&);

    template <typename... Args>
    void call_lua_function(size_t output_count, Args&&... args);

    // TODO:Find better way of doing this
    template <typename T, typename... Args>
    void insert_function_parameters(T&& value, Args&&... args);

    template <typename T>
    void insert_function_parameters(T&& value);

    void insert_function_parameters() { return; }

    template <typename... Results, std::size_t... I>
    std::tuple<Results...> get_function_result(std::index_sequence<I...>&&);
};

template <typename T>
void var_ref::assign(T user_f)
{
    using namespace utilitybz;
    std::function<int(lua_State*)> f = [this, user_function = std::move(user_f)](lua_State*) {
        constexpr std::size_t args_count = callable_traits<T>::args_count;
        bool correct_number_of_arguments = (lua_gettop(m_state) == args_count);

        if (correct_number_of_arguments) {
            std::pair<callable_return_type_t<T>, callable_args_types_t<T>> t{};
            return call_registered_function(user_function, t,
                                            std::make_index_sequence<args_count>());
        }
        return 0;  // Return values count
    };
    insert_CFunction(std::move(f));
}

template <typename Class, typename ReturnType, typename... Args>
void var_ref::assign(Class* obj, ReturnType (Class::*member)(Args...))
{
    std::function<int(lua_State*)> f = [this, object = obj,
                                        member_function = std::move(member)](lua_State*) {
        constexpr std::size_t args_count = sizeof...(Args);
        bool correct_number_of_arguments = (lua_gettop(m_state) == args_count);

        if (correct_number_of_arguments) {
            std::pair<ReturnType, std::tuple<Args...>> t{};
            return call_registered_function(object, member_function, t,
                                            std::make_index_sequence<args_count>());
        }
        return 0;  // Return values count
    };
    insert_CFunction(std::move(f));
}

template <typename ReturnType, typename... Args>
void var_ref::assign(std::function<ReturnType(Args...)> user_f)
{
    std::function<int(lua_State*)> f = [this,
                                        user_function = std::move(user_f)](lua_State*) -> int {
        constexpr std::size_t args_count = sizeof...(Args);
        bool correct_number_of_arguments = (lua_gettop(m_state) == args_count);
        if (correct_number_of_arguments) {
            std::pair<ReturnType, std::tuple<Args...>> t{};
            return call_registered_function(user_function, t,
                                            std::make_index_sequence<args_count>());
        }
        return 0;  // Return values count
    };
    insert_CFunction(std::move(f));
}

template <typename ReturnType, typename... Args>
void var_ref::assign(ReturnType (*user_f)(Args...))
{
    assign(std::function<ReturnType(Args...)>(user_f));
}

// Operators

template <typename T>
var_ref::operator T() const
{
    var_loader loader(m_state, m_name);
    T result = value<typename std::decay<T>::type>::get(m_state, -1);
    return result;
}

template <typename... Args>
var_ref var_ref::operator()(Args&&... args)
{
    constexpr size_t output_count = 1;
    var_loader loader(m_state, m_name);

    call_lua_function(output_count, std::forward<Args>(args)...);
    // TODO:Do i need to generate name here ?
    auto return_value_name = generate_return_value_name();

    lua_setfield(m_state, LUA_GLOBALSINDEX, return_value_name.c_str());
    return var_ref(m_state, return_value_name);
}

template <typename... Results, typename... Args>
std::tuple<Results...> var_ref::call(Args&&... args)
{
    constexpr size_t output_count = sizeof...(Results);
    var_loader loader(m_state, m_name);

    call_lua_function(output_count, std::forward<Args>(args)...);
    return get_function_result<Results...>(std::make_index_sequence<output_count>());
}

template <typename T>
var_ref& var_ref::operator=(T new_value)
{
    var_loader loader(m_state, m_name);
    value<T>::insert(m_state, new_value);
    set_lua_var();
    return *this;
}

template <typename T>
bool var_ref::operator==(const T& rhs) const
{
    T lhs = *this;
    return lhs == rhs;
}

template <typename T>
bool var_ref::operator<(const T& rhs) const
{
    T lhs = *this;
    return lhs < rhs;
}

template <typename T>
bool var_ref::operator!=(const T& rhs) const
{
    return !(*this == rhs);
}

template <typename T>
bool var_ref::operator>(const T& rhs) const
{
    T lhs = *this;
    return rhs < lhs;
}

template <typename T>
bool var_ref::operator<=(const T& rhs) const
{
    return !(this->operator>(rhs));
}

template <typename T>
bool var_ref::operator>=(const T& rhs) const
{
    return !(this->operator<(rhs));
}

template <typename T>
auto var_ref::operator+(const T& rhs) const -> decltype(rhs + rhs)
{
    T lhs = *this;
    return lhs + rhs;
}

template <typename T>
var_ref& var_ref::operator+=(const T& rhs)
{
    // TODO:Temporary generated here
    // This should implement the logic and operator+ should call this one
    this->operator=(static_cast<T>(this->operator+(rhs)));
    return *this;
}

template <typename T>
auto var_ref::operator-(const T& rhs) const -> decltype(rhs - rhs)
{
    T lhs = *this;
    return lhs - rhs;
}

template <typename T>
var_ref& var_ref::operator-=(const T& rhs)
{
    this->operator=(static_cast<T>(this->operator-(rhs)));
    return *this;
}

template <typename T>
auto var_ref::operator/(const T& rhs) const -> decltype(rhs / rhs)
{
    T lhs = *this;
    return lhs / rhs;
}

template <typename T>
var_ref& var_ref::operator/=(const T& rhs)
{
    this->operator=(static_cast<T>(this->operator/(rhs)));
    return *this;
}

template <typename T>
auto var_ref::operator*(const T& rhs) const -> decltype(rhs * rhs)
{
    T lhs = *this;
    return lhs * rhs;
}

template <typename T>
var_ref& var_ref::operator*=(const T& rhs)
{
    this->operator=(static_cast<T>(this->operator*(rhs)));
    return *this;
}

// Helpers
template <typename T, typename ReturnType, typename... Args, std::size_t... I>
int var_ref::call_registered_function(T& user_f,
                                      std::pair<ReturnType, std::tuple<Args...>>&,
                                      std::index_sequence<I...>&&)
{
    std::negate<int> negate;
    auto result = user_f(value<typename std::decay<Args>::type>::get(
        m_state, negate((sizeof...(Args) - static_cast<int>(I))))...);
    value<decltype(result)>::insert(m_state, result);
    return 1;
}
template <typename C, typename F, typename ReturnType, typename... Args, std::size_t... I>
int var_ref::call_registered_function(C obj,
                                      F user_f,
                                      std::pair<ReturnType, std::tuple<Args...>>&,
                                      std::index_sequence<I...>&&)
{
    std::negate<int> negate;
    auto result = (obj->*user_f)(value<typename std::decay<Args>::type>::get(
        m_state, negate((sizeof...(Args) - static_cast<int>(I))))...);
    value<decltype(result)>::insert(m_state, result);
    return 1;
}

template <typename... Args>
void var_ref::call_lua_function(size_t output_count, Args&&... args)
{
    if (!lua_isfunction(m_state, -1)) {
        error("You are trying to call something that is not "
              "neither Lua function nor C/C++ function");
    }
    constexpr int input_count = sizeof...(Args);
    insert_function_parameters(std::forward<Args>(args)...);
    // Note: There is no need to increment used_stack_spaces for
    // the arguments because lua_pcall automatically pops them
    if (lua_pcall(m_state, input_count, output_count, 0) != 0) {
        error(lua_tostring(m_state, -1));
    }
}

template <typename T, typename... Args>
void var_ref::insert_function_parameters(T&& value, Args&&... args)
{
    // TODO:Would this introduce temporaries?
    luabz::value<typename std::decay<T>::type>::insert(m_state, value);
    insert_function_parameters(std::forward<Args>(args)...);
}

template <typename T>
void var_ref::insert_function_parameters(T&& value)
{
    luabz::value<typename std::decay<T>::type>::insert(m_state, value);
}

template <typename... Results, std::size_t... I>
std::tuple<Results...> var_ref::get_function_result(std::index_sequence<I...>&&)
{
    std::negate<int> negate;
    return std::make_tuple(value<typename std::decay<Results>::type>::get(
        m_state, negate((sizeof...(Results) - static_cast<int>(I))))...);
}
}  // namespace luabz