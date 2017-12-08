#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP
#include <string>
inline std::string construct_script_path(std::string script_name)
{
    const std::string lua_extension = ".lua";
    bool has_no_extension =
        script_name.find(lua_extension) == std::string::npos;
    if (has_no_extension) {
        script_name += lua_extension;
    }
    const std::string script_folder = "tests/lua_scripts/";
    return script_folder + script_name;
}
#endif