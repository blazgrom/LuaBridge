#include <functional>
#include <string>
#include "gtest/gtest.h"

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
// TODO:
// Add tests with parameters and different return types
// Call lua function, not registered function
TEST_F(lua_value_Function, AssignAndCallStdFunction)
{
    auto result = false;
    std::function<int()> f = [&]() {
        result = true;
        return 100;
    };
    script["stdfunction"].calls(f);
    int return_value = script["stdfunction"]();
    ASSERT_TRUE(return_value == 100);
}
int normal_function()
{
    return 77;
}
TEST_F(lua_value_Function, AssignAndCallNormalFunction)
{
    script["normal_function"].calls(normal_function);
    int return_value = script["normal_function"]();
    ASSERT_TRUE(return_value == 77);
}
TEST_F(lua_value_Function, AssignAndCallCapturelessLambda)
{
    script["lambda"].calls([]() mutable { return 666; });
    int return_value = script["lambda"]();
    ASSERT_TRUE(return_value == 666);
}
TEST_F(lua_value_Function, AssignAndCallCaptureLambda)
{
    int r = 777;
    script["lambda_capture"].calls([r]() mutable { return r; });
    int return_value = script["lambda_capture"]();
    ASSERT_TRUE(return_value == r);
}
TEST_F(lua_value_Function, CallCapturelessLambdaWithArgument)
{
    script["lambda"].calls([](int a) mutable { return a; });
    int return_value = script["lambda"](1000);
    ASSERT_TRUE(1000 == return_value);
}