#include "lua_test_helpers.hpp"
#include "luabz.hpp"
#include <gtest/gtest.h>
#include <string>

class value_Operator : public ::testing::Test
{
  public:
    luabz::script script{construct_script_path("luascript_test.lua")};
    void TearDown() override { script.close(); }
};
TEST_F(value_Operator, OperatorEqualsBetweenTwoLuaValuesFromSameFile)
{
    ASSERT_TRUE(script["double_var"] == script["double_var"]);
}
TEST_F(value_Operator, OperatorEqualsBetweenTwoLuaValuesFromDifferentFile)
{
    luabz::script script2{construct_script_path("luascript_test2.lua")};
    ASSERT_FALSE(script["double_var"] == script2["double_var"]);
    script2.close();
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndBool)
{
    ASSERT_TRUE(script["boolt_var"] == true);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstBool)
{
    const bool expected = true;
    ASSERT_TRUE(script["boolt_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndInt)
{
    int expected = 100;
    int actual = script["integer_var"];
    ASSERT_TRUE(actual == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstInt)
{
    const int expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndUnsignedInt)
{
    unsigned int expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstUnsignedInt)
{
    const unsigned int expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndLong)
{
    long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstLong)
{
    const long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndUnsignedLong)
{
    unsigned long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstUnsignedLong)
{
    const unsigned long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndLongLong)
{
    long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstLongLong)
{
    const long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndUnsignedLongLong)
{
    unsigned long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstUnsignedLongLong)
{
    const unsigned long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndFloat)
{
    float expected = 102.0351f;
    ASSERT_TRUE(script["double_var"] == expected);
}

TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstFloat)
{
    const float expected = 102.0351f;
    ASSERT_TRUE(script["double_var"] == expected);
}

TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndDouble)
{
    double expected = 102.0351;
    ASSERT_TRUE(script["double_var"] == expected);
}

TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstDouble)
{
    const double expected = 102.0351;
    ASSERT_TRUE(script["double_var"] == expected);
}
TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndChar)
{
    char expected = 'T';
    ASSERT_TRUE(script["string_var"] == expected);
}

TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstChar)
{
    const char expected = 'T';
    ASSERT_TRUE(script["string_var"] == expected);
}

TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndString)
{
    std::string expected = "This is some string";
    ASSERT_TRUE(script["string_var"] == expected);
}

TEST_F(value_Operator, OperatorEqualsBetweenLuaValueAndConstString)
{
    const std::string expected = "This is some string";
    ASSERT_TRUE(script["string_var"] == expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenTwoLuaValuesFromSameFile)
{
    ASSERT_TRUE(script["double_var"] < script["double_var2"]);
}
TEST_F(value_Operator, OperatorLessThanBetweenTwoLuaValuesFromDifferentFile)
{
    luabz::script script2{construct_script_path("luascript_test2.lua")};
    ASSERT_FALSE(script["double_var"] < script2["double_var"]);
    script2.close();
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndBool)
{
    ASSERT_TRUE(script["boolf_var"] < true);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstBool)
{
    const bool expected = true;
    ASSERT_TRUE(script["boolf_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndInt)
{
    int expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstInt)
{
    const int expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndUnsignedInt)
{
    unsigned int expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstUnsignedInt)
{
    const unsigned int expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndLong)
{
    long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstLong)
{
    const long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndUnsignedLong)
{
    unsigned long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstUnsignedLong)
{
    const unsigned long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndLongLong)
{
    long long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstLongLong)
{
    const long long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndUnsignedLongLong)
{
    unsigned long long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstUnsignedLongLong)
{
    const unsigned long long expected = 101;
    ASSERT_TRUE(script["integer_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndFloat)
{
    float expected = 105.0351f;
    ASSERT_TRUE(script["double_var"] < expected);
}

TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstFloat)
{
    const float expected = 105.0351f;
    ASSERT_TRUE(script["double_var"] < expected);
}

TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndDouble)
{
    double expected = 105.0351;
    ASSERT_TRUE(script["double_var"] < expected);
}

TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstDouble)
{
    const double expected = 105.0351;
    ASSERT_TRUE(script["double_var"] < expected);
}
TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndChar)
{
    char expected = 'Z';
    ASSERT_TRUE(script["string_var"] < expected);
}

TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstChar)
{
    const char expected = 'Z';
    ASSERT_TRUE(script["string_var"] < expected);
}

TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndString)
{
    std::string expected = "Zhis is some string";
    ASSERT_TRUE(script["string_var"] < expected);
}

TEST_F(value_Operator, OperatorLessThanBetweenLuaValueAndConstString)
{
    const std::string expected = "Zhis is some string";
    ASSERT_TRUE(script["string_var"] < expected);
}