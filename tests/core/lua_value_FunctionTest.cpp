#include <gtest/gtest.h>
#include <functional>
#include <string>
#include "lua_script.hpp"
#include "lua_test_helpers.hpp"
class lua_value_Function : public ::testing::Test
{
  public:
    luabz::lua_script script;
    void SetUp() override
    {
        std::string lua_script_file =
            construct_script_path("luascript_test.lua");
        script.open(lua_script_file);
    }
};
//TODO:
//Add tests with parameters and different return types
//Call lua function, not registered function
TEST_F(lua_value_Function, AssignAndCallStdFunction)
{
    auto result = false;
    std::function<int()> f = [&]() {
        result = true;
        return 100;
    };
    script["stdfunction"] = f;
    int return_value = script["stdfunction"]();
    ASSERT_TRUE(return_value == 100);
}
int normal_function()
{
    return 77;
}
TEST_F(lua_value_Function,AssignAndCallNormalFunction)
{
    script["normal_function"]=normal_function;
    int return_value=script["normal_function"]();
    ASSERT_TRUE(return_value==77);
}