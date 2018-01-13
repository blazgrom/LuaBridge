#include <string>
#include "gtest/gtest.h"
#include "lua_script.hpp"
#include "lua_test_helpers.hpp"

class lua_value_Test : public ::testing::Test
{
  public:
        luabz::lua_script script{construct_script_path("luascript_test.lua")};

};

TEST_F(lua_value_Test,
       TheSameVariableIsVisibleFromTwoDifferentLuaScriptsOnTheSameFile)
{
    std::string new_value = "This is the new value for the lua string";
    std::string lua_script_file = construct_script_path("luascript_test.lua");
    luabz::lua_script second_script(lua_script_file);
    script["string_var"] = new_value;
    std::string first_script_value = script["string_var"];
    std::string second_script_value = second_script["string_var"];
    ASSERT_TRUE(first_script_value == second_script_value);
}
TEST_F(lua_value_Test, CheckIfNotNilVarIsNil)
{
    ASSERT_FALSE(script["string_var"].is_nil());
}
TEST_F(lua_value_Test, CheckIfNilVarIsNil)
{
    ASSERT_TRUE(script["nil_var"].is_nil());
}
