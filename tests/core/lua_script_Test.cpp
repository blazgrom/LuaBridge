#include "core/lua_script.hpp"
#include <iostream>
#include <string>
#include <gtest/gtest.h>
using namespace ::testing;
using namespace LuaBz;
using namespace std;

class lua_scriptF : public Test
{
  public:
    LuaBz::lua_script script;
    void SetUp() override
    {
        std::string lua_script_file = "../tests/core/luascript_test.lua";
        script.open(lua_script_file);
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
// TEST_F(lua_scriptF, GetLuaTable)
// {
//     lua_table position = script.get<lua_table>("Position");
//     int expected = 1;
//     int actual = position["x"];
//     ASSERT_EQ(expected, actual);
// }
///Test ideas
///1 SetLuaIntegerWithX where x is any integral type
///2 SetLuaNumberWithX where x is any integral type
///3 SetLuaBoolWithX where x is anything that can be converted to bool

TEST_F(lua_scriptF, ExecuteLuaCodeFromCppString)
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