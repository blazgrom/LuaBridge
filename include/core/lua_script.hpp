#ifndef LUABZ_LUA_SCRIPT_HPP
#define LUABZ_LUA_SCRIPT_HPP
#include <cassert>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include "callable_traits.hpp"
#include "lua.hpp"
#include "lua_function.hpp"
#include "lua_stack.hpp"
#include "variadic_index.hpp"
#include "various.hpp"
namespace LuaBz
{
class lua_script
{
  private:
    using LuaCF_Intermediary = std::function<int(lua_State *)>;
    lua_stack m_stack;
    std::string m_fileName;
    bool m_open;
    static std::vector<LuaCF_Intermediary> m_registeredFunctions;

  public:
    lua_script();
    explicit lua_script(const std::string &file, bool loadStandardLib = false);
    lua_script(const std::string &file,
               const std::vector<std::string> &dependencies,
               bool loadStandardLib = false);
    lua_script(const std::string &file,
               const std::string &dependency,
               bool loadStandardLib = false);
    ~lua_script() = default;
    lua_script(const lua_script &rhs) = delete;
    lua_script &operator=(const lua_script &rhs) = delete;
    lua_script(lua_script &&rhs) = default;
    lua_script &operator=(lua_script &&rhs) = default;
    void open(const std::string &file,
              std::vector<std::string> dependencies = {},
              bool loadStandardLib = false);
    void close() noexcept;
    bool change(const std::string &newFile) noexcept;
    template <class T>
    T get(const std::string &name) const;
    template <class T>
    void set(const std::string &name, T &&val) const;
    template <class... R, class... Args>
    std::tuple<R...> call(const lua_function<R...> &f, Args &&... args) const;
    template <class First, class Second, class... Args>
    std::pair<First, Second> call(const lua_function<First, Second> &f,
                                  Args &&... args) const;
    template <class T, class... Args>
    T call(const lua_function<T> &f, Args &&... args) const;
    template <class... Args>
    void call(const lua_function<void> &f, Args &&... args) const;
    template <class... T>
    std::tuple<T...> call(const lua_function<T...> &f) const;
    template <class T>
    T call(const lua_function<T> &f) const;
    template <class First, class Second>
    std::pair<First, Second> call(const lua_function<First, Second> &f) const;
    void call(const lua_function<void> &f) const;
    void run(std::string luaCode);
    template <class R, class... Args>
    void register_function(const std::string &name,
                           std::function<R(Args...)> &user_f);
    template <class R, class... Args>
    void register_function(const std::string &name, R (*user_f)(Args...));
    template <class T>
    void register_function(const std::string &name, T &user_f);
    void operator[](const std::string &name) const;

  private:
    void load_function(const std::string &name) const;
    template <class... Args>
    std::tuple<Args...> get_output(unsigned int count) const;
    template <class T>
    T get_output_impl(int index) const;
    template <class T, class... Args>
    int set_parameters(T &&value, Args &&... args) const;
    template <class T>
    int set_parameters(T &&value) const;
    void register_function_impl(const std::string &name);
    template <class T, class R, class... Args>
    int call_registered_function(T &user_f,
                                 Utils::type_container<R>,
                                 std::tuple<Args...> &);
    template <class T, class... Args>
    int call_registered_function(T &user_f,
                                 Utils::type_container<void>,
                                 std::tuple<Args...> &);
};
#ifndef LUA_SCRIPT_TEMPLATE_IMPL
#define LUA_SCRIPT_TEMPLATE_IMPL
#include "lua_script.tpp"
#undef LUA_SCRIPT_TEMPLATE_IMPL
#endif
}
#endif  // !LUABZ_LUA_SCRIPT_HPP