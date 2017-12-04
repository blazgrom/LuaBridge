#include <gtest/gtest.h>
#include <string>
#include "lua_script.hpp"

class lua_value_Operator : public ::testing::Test
{
  public:
    luabz::lua_script script;
    void SetUp() override
    {
        std::string lua_script_file = "../tests/lua_scripts/luascript_test.lua";
        script.open(lua_script_file);
    }
};
TEST_F(lua_value_Operator, OperatorEqualsBetweenTwoLuaValuesFromSameFile)
{
    ASSERT_TRUE(script["double_var"] == script["double_var"]);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenTwoLuaValuesFromDifferentFile)
{
    luabz::lua_script script2;
    script2.open("../tests/lua_scripts/luascript_test2.lua");
    ASSERT_FALSE(script["double_var"] == script2["double_var"]);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndBool)
{
    ASSERT_TRUE(script["boolt_var"] == true);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstBool)
{
    const bool expected = true;
    ASSERT_TRUE(script["boolt_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndShort)
{
    short expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstShort)
{
    const short expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndUnsignedShort)
{
    unsigned short expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstUnsignedShort)
{
    const unsigned short expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndInt)
{
    int expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstInt)
{
    const int expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndUnsignedInt)
{
    unsigned int expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstUnsignedInt)
{
    const unsigned int expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndLong)
{
    long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstLong)
{
    const long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndUnsignedLong)
{
    unsigned long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstUnsignedLong)
{
    const unsigned long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndLongLong)
{
    long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstLongLong)
{
    const long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndUnsignedLongLong)
{
    unsigned long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator,
       OperatorEqualsBetweenLuaValueAndConstUnsignedLongLong)
{
    const unsigned long long expected = 100;
    ASSERT_TRUE(script["integer_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndFloat)
{
    float expected = 102.0351f;
    ASSERT_TRUE(script["double_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstFloat)
{
    const float expected = 102.0351f;
    ASSERT_TRUE(script["double_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndDouble)
{
    double expected = 102.0351;
    ASSERT_TRUE(script["double_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstDouble)
{
    const double expected = 102.0351;
    ASSERT_TRUE(script["double_var"] == expected);
}
TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndChar)
{
    char expected = 'T';
    ASSERT_TRUE(script["string_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstChar)
{
    const char expected = 'T';
    ASSERT_TRUE(script["string_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndString)
{
    std::string expected = "This is some string";
    ASSERT_TRUE(script["string_var"] == expected);
}

TEST_F(lua_value_Operator, OperatorEqualsBetweenLuaValueAndConstString)
{
    const std::string expected = "This is some string";
    ASSERT_TRUE(script["string_var"] == expected);
}