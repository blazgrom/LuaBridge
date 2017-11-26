#include "core/lua_script.hpp"
#include <iostream>
#include <string>
#include "gtest/gtest.h"
using namespace ::testing;
using namespace LuaBz;
using namespace std;

class lua_scriptF : public Test
{
  public:
    string main_script_dep;
    string main_script_dep2;
    string main_script;
    string script_folder_path;
    string first_dependecy_string;
    std::string second_dependency_string;
    LuaBz::lua_script script;
    void SetUp() override
    {
        script_folder_path = "../tests/core/";
        main_script_dep = script_folder_path + "luascript_dep.lua";
        main_script_dep2 = script_folder_path + "luascript_dep2.lua";
        main_script = script_folder_path + "luascript_test.lua";
        first_dependecy_string = "This is lua script dependency";
        second_dependency_string = "This is a second dependency";
        std::vector<string> dependencies{main_script_dep, main_script_dep2};
        script.open(main_script, dependencies);
    }
};
// TEST_F(lua_scriptF, LoadingDependencyAndMainScript)
// {
//     ASSERT_TRUE(first_dependecy_string == script.get<string>("dependency"));
// }
// TEST_F(lua_scriptF, LoadingMultipleDependenciesAndMainScript)
// {
//     ASSERT_TRUE(first_dependecy_string == script.get<string>("dependency"));
//     ASSERT_TRUE(second_dependency_string == script.get<string>("dependency2"));
// }
///Test ideas
///1 SetLuaIntegerWithX where x is any integral type
///2 SetLuaNumberWithX where x is any integral type
///3 SetLuaBoolWithX where x is anything that can be converted to bool

TEST_F(lua_scriptF, GetLuaBool)
{
    ASSERT_TRUE(script["boolt_var"]);
    ASSERT_FALSE(script["boolf_var"]);
}
TEST_F(lua_scriptF, GetLuaString)
{
    string expected = "This is some string";
    std::string actual=script["string_var"];
    ASSERT_TRUE(expected == actual);
}
TEST_F(lua_scriptF, GetLuaInteger)
{
    int integer_var=script["integer_var"];
    ASSERT_EQ(100,integer_var);
}
TEST_F(lua_scriptF, GetLuaNumber)
{
    double double_var=script["double_var"];
    ASSERT_DOUBLE_EQ(102.0351, double_var);
}
TEST_F(lua_scriptF, SetLuaBool)
{
    script["boolt_var"]=false;
    ASSERT_FALSE(script["boolt_var"]);
}
TEST_F(lua_scriptF, SetLuaString)
{
    std::string new_value = "This is the new value for the lua string";
    script["string_var"]=new_value;
    std::string actual=script["string_var"];
    ASSERT_TRUE(new_value == actual);
}
TEST_F(lua_scriptF, SetLuaInteger)
{
    script["integer_var"]=666;
    int integer_var=script["integer_var"];
    ASSERT_EQ(666, integer_var);
}
TEST_F(lua_scriptF, SetLuaNumber)
{
    script["double_var"]=10.132265;
    double double_var=script["double_var"];
    ASSERT_DOUBLE_EQ(10.132265, double_var);
}
TEST_F(lua_scriptF, ExecuteLuaCode)
{
    string code = R"(variable=1;)"
                  R"(function increment(input) )"
                  R"(    input=input+1;)"
                  R"(    return input;)"
                  R"(end )"
                  R"(variable=increment(variable) )";
    script(code);
    int variable=script["variable"];
    ASSERT_EQ(2, variable);
}

// TEST_F(lua_scriptF, GetLuaTable)
// {
//     lua_table position = script.get<lua_table>("Position");
//     int expected = 1;
//     int actual = position["x"];
//     ASSERT_EQ(expected, actual);
// }