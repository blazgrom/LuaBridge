#include "lua_test_helpers.hpp"
#include "luabz.hpp"
#include <gtest/gtest.h>
#include <string>

class value_Test : public ::testing::Test
{
  public:
    luabz::script script{construct_script_path("luascript_test.lua")};
    void TearDown() override { script.close(); }
};

TEST_F(value_Test, TheSameVariableIsVisibleFromTwoDifferentLuaScriptsOnTheSameFile)
{
    std::string new_value = "This is the new value for the lua string";
    std::string script_file = construct_script_path("luascript_test.lua");
    luabz::script second_script(script_file);
    script["string_var"] = new_value;
    std::string first_script_value = script["string_var"];
    std::string second_script_value = second_script["string_var"];
    ASSERT_TRUE(first_script_value == second_script_value);
}
TEST_F(value_Test, CheckIfNotNilVarIsNil)
{
    ASSERT_FALSE(script["string_var"].is_nil());
}
TEST_F(value_Test, CheckIfNilVarIsNil)
{
    ASSERT_TRUE(script["nil_var"].is_nil());
}
