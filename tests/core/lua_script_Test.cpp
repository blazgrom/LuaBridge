#include "lua_test_helpers.hpp"
#include "luabz.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
class scriptF : public ::testing::Test
{
  public:
    luabz::script script{construct_script_path("luascript_test.lua")};
    void TearDown() override { script.close(); }
};
TEST_F(scriptF, ExecuteLuaCodeFromCppString)
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
TEST_F(scriptF, CheckIfVariablesIsNil)
{
    bool result = script["nil_var"].is_nil();
    ASSERT_TRUE(result);
}
TEST_F(scriptF, CheckIfLuaStdIsCorrectlyLoaded)
{
    script.open_std();
    script("variable=math.floor(1213.23)");
    int variable = script["variable"];
    ASSERT_EQ(1213, variable);
}