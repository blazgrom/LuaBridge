#include "core/lua_script.hpp"
#include <gtest/gtest.h>
#include <string>
class lua_scriptF : public ::testing::Test
{
  public:
    LuaBz::lua_script script;
    void SetUp() override
    {
        std::string lua_script_file = "../tests/lua_scripts/luascript_test.lua";
        script.open(lua_script_file);
    }
};
TEST_F(lua_scriptF, ExecuteLuaCodeFromCppString)
{
    std::string code = R"(variable=1;)"
                       R"(function increment(input) )"
                       R"(    input=input+1;)"
                       R"(    return input;)"
                       R"(end )"
                       R"(variable=increment(variable) )";
    script(code);
    int variable = script["variable"];
    ASSERT_EQ(2, variable);
}