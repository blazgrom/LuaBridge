#ifndef luabz_LUA_SCRIPT_HPP
#define luabz_LUA_SCRIPT_HPP
#include <cassert>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include "lua.hpp"
#include "lua_value_ref.hpp"
namespace luabz
{
class lua_script
{
  private:
    lua_State *m_state;
    std::string m_fileName;
    bool m_open;

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
    void operator()(const std::string &lua_code) const;
    lua_value_ref operator[](const std::string &name) const;
};
}
#endif  // !luabz_LUA_SCRIPT_HPP