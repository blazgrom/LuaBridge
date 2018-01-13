#include "lua_script.hpp"
#include <string>
#include "gtest/gtest.h"
#include "lua_test_helpers.hpp"

class lua_scriptF : public ::testing::Test
{
  public:
    luabz::lua_script script{construct_script_path("luascript_test.lua")};
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
TEST_F(lua_scriptF, CheckIfVariablesIsNil)
{
    bool result = script["nil_var"].is_nil();
    ASSERT_TRUE(result);
}
TEST_F(lua_scriptF, CheckIfLuaStdIsCorrectlyLoaded)
{
    luabz::lua_script current_script(
        construct_script_path("luascript_test.lua"));
    current_script.open_std();
    current_script("variable=math.floor(1213.23)");
    int variable = current_script["variable"];
    ASSERT_EQ(1213, variable);
}
TEST_F(lua_scriptF, CheckIfClosingAStateClearsAllDataAssociatedWithTheState)
{
    int a = 102;
    script["simple_var"] = a;
    //  script.close();
    //  script.open();
    //  script("simple_var_exists=simple_var==102");
    bool simple_var_exists = script["simple_var_exists"];
    ASSERT_FALSE(false);
}