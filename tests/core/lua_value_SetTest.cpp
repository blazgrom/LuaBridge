#include <string>
#include <gtest/gtest.h>
#include "core/lua_script.hpp"

class lua_value_Set : public ::testing::Test
{
  public:
    LuaBz::lua_script script;
    void SetUp() override
    {
        std::string lua_script_file = "../tests/lua_scripts/luascript_test.lua";
        script.open(lua_script_file);
    }
};
TEST_F(lua_value_Set, LuaVarFromCppBool)
{
    script["boolt_var"]=false;
    ASSERT_FALSE(script["boolt_var"]);
}
TEST_F(lua_value_Set,LuaVarFromCppBoolExpression)
{
    script["boolt_var"]=(1==0);
    ASSERT_FALSE(script["boolt_var"]);        
}
TEST_F(lua_value_Set, LuaVarFromCppString)
{
    std::string new_value = "This is the new value for the lua string";
    script["string_var"]=new_value;
    std::string actual=script["string_var"];
    ASSERT_TRUE(new_value == actual);
}
TEST_F(lua_value_Set,LuaVarFromCppChar)
{
    char expected='Z';
    script["string_var"]=expected;
    char actual=script["string_var"];
    ASSERT_TRUE(expected==actual);
}

TEST_F(lua_value_Set, LuaVarFromCppLongLong)
{
    long expected=666;
    script["integer_var"]=expected;
    long long actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}

TEST_F(lua_value_Set, LuaVarFromCppUnsignedLongLong)
{
    unsigned long long expected=666;
    script["integer_var"]=expected;
    unsigned long long actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}
TEST_F(lua_value_Set, LuaVarFromCppLong)
{
    long expected=666;
    script["integer_var"]=expected;
    long actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}

TEST_F(lua_value_Set, LuaVarFromCppUnsignedLong)
{
    unsigned long expected=666;
    script["integer_var"]=expected;
    unsigned long actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}
TEST_F(lua_value_Set, LuaVarFromCppInteger)
{
    int expected=666;
    script["integer_var"]=expected;
    int actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}

TEST_F(lua_value_Set, LuaVarFromCppUnsignedInteger)
{
    unsigned int expected=666;
    script["integer_var"]=expected;
    unsigned int actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}

TEST_F(lua_value_Set, LuaVarFromCppShort)
{
    short expected=666;
    script["integer_var"]=expected;
    short actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}

TEST_F(lua_value_Set, LuaVarFromCppUnsignedShort)
{
    unsigned short expected=666;
    script["integer_var"]=expected;
    unsigned short actual=script["integer_var"];
    ASSERT_EQ(expected,actual);
}
TEST_F(lua_value_Set, LuaVarFromCppDouble)
{
    script["double_var"]=10.132265;
    double double_var=script["double_var"];
    ASSERT_DOUBLE_EQ(10.132265, double_var);
}

TEST_F(lua_value_Set, LuaVarFromCppFloat)
{
    float expected=10.132265f;
    script["double_var"]=expected;
    float actual=script["double_var"];
    ASSERT_FLOAT_EQ(expected, actual);
}