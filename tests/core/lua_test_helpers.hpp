#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP
#include <cstdio>
#include <string>
// TOOD (blazgrom): Move this file to detail,and delete the folder core. The
// test folder structure should be identical to the structure of the include/src
inline std::string construct_script_path(std::string script_name)
{
    const std::string lua_extension = ".lua";
    bool has_no_extension =
        script_name.find(lua_extension) == std::string::npos;
    if (has_no_extension) {
        script_name += lua_extension;
    }
    // TODO (blazgrom) : find a better way of defining the base folder
    auto project_folder = LUABZ_PROJECT_FOLDER_PATH;
    const std::string script_folder = "/tests/lua_scripts/";
    return project_folder + script_folder + script_name;
}
#endif