#include "gtest/gtest.h"
#include <functional>
#include <string>
#include <tuple>

#include "luabz.hpp"
#include "lua_test_helpers.hpp"
class value_Function : public ::testing::Test
{
  public:
    luabz::script script{construct_script_path("luascript_test.lua")};
    void TearDown() override { script.close(); }
};

TEST_F(value_Function, AfterClosingTheLuaScriptTheRegisteredFunctionsAreRemoved)
{
    script["lambda"].assign([](int a)  { return a; });
    script["lambda"](1);
    script.close();
    script.open();
    script["lambda"].assign([](int a)  { return a * 2; });
    int return_value = script["lambda"](1000);
    ASSERT_TRUE(2000 == return_value);
}
TEST_F(value_Function, AssignAndassigntdFunction)
{
    auto result = false;
    std::function<int()> f = [&]() {
        result = true;
        return 100;
    };
    script["stdfunction"].assign(f);
    int return_value = script["stdfunction"]();
    ASSERT_TRUE(return_value == 100);
}
int normal_function()
{
    return 77;
}
TEST_F(value_Function, AssignAndCallNormalFunction)
{
    script["normal_function"].assign(normal_function);
    int return_value = script["normal_function"]();
    ASSERT_TRUE(return_value == 77);
}
TEST_F(value_Function, AssignAndCallCapturelessLambda)
{
    script["lambda"].assign([]()  { return 666; });
    int return_value = script["lambda"]();
    ASSERT_TRUE(return_value == 666);
}
TEST_F(value_Function, AssignAndCallCaptureLambda)
{
    int r = 777;
    script["lambda_capture"].assign([r]()  { return r; });
    int return_value = script["lambda_capture"]();
    ASSERT_TRUE(return_value == r);
}
TEST_F(value_Function, CallCapturelessLambdaWithArgument)
{
    script["lambda"].assign([](int a) { return a; });
    int return_value = script["lambda"](1000);
    ASSERT_TRUE(1000 == return_value);
}
struct T {
    int f(int a) { return 1111 * a; }
};
TEST_F(value_Function, CallMemberFunction)
{
    T s;
    script["member_function"].assign(&s, &T::f);
    int return_value = script["member_function"](2);
    ASSERT_TRUE(2222 == return_value);
}
TEST_F(value_Function, CallLuaFunctionWithSingleReturn)
{
    int argument = 125;
    auto result = script["single_return"].call<int>(argument);
    ASSERT_TRUE(argument == std::get<0>(result));
}
TEST_F(value_Function, CallLuaFunctionWithMultipleReturns)
{
    int a = 125;
    int b = 250;
    auto result = script["multiple_returns"].call<int, int>(a, b);
    ASSERT_TRUE(a == std::get<0>(result) && b == std::get<1>(result));
}