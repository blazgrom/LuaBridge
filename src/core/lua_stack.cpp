#include "core/lua_stack.hpp"
#include "core/lua_type.hpp"
namespace LuaBz
{
using Utils::can_represent_v;
lua_stack::lua_stack() : m_state{nullptr} {}
lua_stack::lua_stack(bool loadStandardLib,
                     const std::string &file,
                     std::vector<std::string> dependencies)
  : m_state{nullptr}
{
    create(file, dependencies, loadStandardLib);
}
lua_stack::~lua_stack()
{
    lua_close(m_state);
}
void lua_stack::pop(int count) const
{
    lua_pop(m_state, count);
}
void lua_stack::set_top_element(const std::string &name) const
{
    if (name.find('.') == std::string::npos) {
        get_global_variable(name);
    } else {
        get_table_element(name);
    }
}
void lua_stack::push(const lua_table &val) const
{
    lua_newtable(m_state);
    for (const auto &element : val) {
        push(element.name());
        switch (element.type()) {
            case lua_types::Boolean:
                push(element.get<lua_t::boolean>());
                break;
            case lua_types::Number:
                push(element.get<lua_t::number>());
                break;
            case lua_types::Integer:
                push(element.get<lua_t::integer>());
                break;
            case lua_types::Nil:
                push(element.get<lua_t::nil>());
                break;
            case lua_types::String:
                push(element.get<lua_t::string>());
                break;
            case lua_types::Table:
                push(element.get<lua_table>());
                break;
        }
        lua_settable(m_state,
                     -3);  // Note: automatically pops the pair [key,value]
    }
}
void lua_stack::push(lua_CFunction function) const
{
    // Note:
    // All C++ function are inserted as closures with one upvalue, this
    // upvalue contains the index in the static element containing all the
    // user defined functions
    const int upvalue_count = 1;
    lua_pushcclosure(m_state, function, upvalue_count);
}
void lua_stack::push(std::nullptr_t) const
{
    lua_pushnil(m_state);
}
void lua_stack::push(std::string val) const
{
    lua_pushlstring(m_state, val.c_str(), val.size());
}
void lua_stack::push(char val) const
{
    std::string s{val};
    push(s);
}
void lua_stack::push(bool val) const
{
    lua_pushboolean(m_state, val);
}
void lua_stack::push(short val) const
{
    auto pre_cond = can_represent_v<short, lua_Integer>;
    assert(pre_cond);
    lua_pushinteger(m_state, val);
}
void lua_stack::push(int val) const
{
    auto pre_cond = can_represent_v<int, lua_Integer>;
    assert(pre_cond);
    lua_pushinteger(m_state, val);
}
void lua_stack::push(unsigned int val) const
{
    auto pre_cond = can_represent_v<unsigned int, lua_Integer>;
    assert(pre_cond);
    lua_pushinteger(m_state, val);
}
void lua_stack::push(long val) const
{
    auto pre_cond = can_represent_v<long, lua_Integer>;
    assert(pre_cond);
    lua_pushinteger(m_state, val);
}
void lua_stack::push(unsigned long val) const
{
    auto pre_cond = can_represent_v<unsigned long, lua_Integer>;
    assert(pre_cond);
    lua_pushinteger(m_state, val);
}
void lua_stack::push(long long val) const
{
    auto pre_cond = can_represent_v<long long, lua_Integer>;
    assert(pre_cond);
    lua_pushinteger(m_state, val);
}
void lua_stack::push(unsigned long long val) const
{
    auto pre_cond = can_represent_v<unsigned long long, lua_Integer>;
    assert(pre_cond);
    lua_pushinteger(m_state, val);
}
void lua_stack::push(float val) const
{
    auto pre_cond = can_represent_v<float, lua_Number>;
    assert(pre_cond);
    lua_pushnumber(m_state, val);
}
void lua_stack::push(double val) const
{
    auto pre_cond = can_represent_v<double, lua_Number>;
    assert(pre_cond);
    lua_pushnumber(m_state, val);
}
void lua_stack::call_function(int inputCount, int outputCount) const
{
    if (lua_pcall(m_state, inputCount, outputCount, 0) != 0) {
        bool popTopElement = true;
        throw lua_error(top_element<std::string>(popTopElement));
    }
}
bool lua_stack::is_function(int index) const
{
    return lua_isfunction(m_state, index);
}
int lua_stack::size() const
{
    return lua_gettop(m_state);
}
void lua_stack::execute(const std::string &code) const
{
    if (luaL_dostring(m_state, code.c_str())) {
        throw lua_error(lua_tostring(m_state, topElement));
    }
}
void lua_stack::destroy()
{
    lua_close(m_state);
}
void lua_stack::create(const std::string &file,
                       const std::vector<std::string> &dependencies,
                       bool loadStandardLib)
{
    m_state = luaL_newstate();
    if (loadStandardLib)
        luaL_openlibs(m_state);
    for (const std::string &depend : dependencies) {
        run_file(depend);
    }
    run_file(file);
}
void lua_stack::run_file(const std::string &name) const
{
    if (luaL_dofile(m_state, name.c_str())) {
        throw lua_error(lua_tostring(m_state, topElement));
    }
}
void lua_stack::get_global_variable(const std::string &name) const
{
    lua_getglobal(m_state, name.c_str());
    if (lua_isnoneornil(m_state, topElement)) {
        throw lua_error("A variable with the name:" + name +
                        " could not be found");
    }
}
void lua_stack::get_table_element(const std::string &name) const
{
    std::string tableName = name.substr(0, name.find_first_of('.')),
                tablefield = name.substr(name.find_first_of('.') + 1);
    get_global_variable(tableName);
    bool keepProcessing = true;
    while (keepProcessing) {
        auto dotPosition = tablefield.find_first_of('.');
        if (dotPosition == std::string::npos) {
            load_table_field(tablefield);
            keepProcessing = false;
        } else {
            std::string parent = tablefield.substr(0, dotPosition);
            tablefield = tablefield.substr(dotPosition + 1);
            load_table_field(parent);
        }
    }
}
void lua_stack::load_table_field(const std::string &name) const
{
    if (!lua_istable(m_state, topElement)) {
        throw lua_error("The field " + name + " could not be loaded");
    }
    lua_getfield(m_state, topElement, name.c_str());
}
lua_table lua_stack::create_lua_table() const
{
    lua_table table;
    auto callback = [&table, this](const std::string &key) {
        switch (lua_type(m_state, topElement)) {
            case LUA_TNIL:
                table.push_back(lua_value(key, nullptr));
                break;
            case LUA_TBOOLEAN:
                table.push_back(lua_value(key, top_element<lua_t::boolean>()));
                break;
            case LUA_TNUMBER: {
                double number = lua_tonumber(m_state, topElement);
                if (number == static_cast<lua_t::integer>(number)) {
                    table.push_back(
                        lua_value(key, top_element<lua_t::integer>()));
                } else {
                    table.push_back(
                        lua_value(key, top_element<lua_t::number>()));
                }
            } break;
            case LUA_TSTRING:
                table.push_back(lua_value(key, top_element<lua_t::string>()));
                break;
            case LUA_TTABLE:
                table.push_back(lua_value(key, top_element<lua_table>()));
                break;
            // Note:TODO
            // lua_value does not support Lua Cfunction so we only
            // pop the value, later on maybe i will add support
            case LUA_TFUNCTION:
                break;
        }
    };
    iterate_table(callback);
    return table;
}
void lua_stack::iterate_table(
    std::function<void(const std::string &)> callback) const
{
    lua_pushnil(m_state);  // Initial key
    const int keyIndex = -2;
    while (lua_next(m_state, keyIndex) != 0) {
        /*key=-2, value=-1*/
        std::string tableKey{};
        if (lua_isstring(m_state, keyIndex) &&
            !lua_isnumber(m_state, keyIndex)) {
            tableKey = lua_tostring(m_state, keyIndex);
        } else if (lua_isnumber(m_state, keyIndex)) {
            tableKey = std::to_string(get<lua_t::integer>(keyIndex));
        }
        callback(tableKey);
        pop();
    }
}
}