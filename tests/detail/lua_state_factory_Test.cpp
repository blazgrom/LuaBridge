#include <string>
#include <gtest/gtest.h>
#include "detail/lua_state_factory.hpp"

TEST(lua_state_factory_Test,CallingFactoryTwiceOnTheSameFileReturnSameLuaState)
{
    std::string lua_file="../tests/lua_scripts/luascript_test.lua";
    lua_State* a=LuaBz::detail::lua_state_factory::create_state(lua_file);
    lua_State* b=LuaBz::detail::lua_state_factory::create_state(lua_file);
    ASSERT_TRUE(a==b);
}
TEST(lua_state_factory_Test,TwoDifferentFileHaveDifferentStates)
{
    std::string file_a="../tests/lua_scripts/luascript_test.lua";
    std::string file_b="../tests/lua_scripts/luascript_test2.lua";
    lua_State* a=LuaBz::detail::lua_state_factory::create_state(file_a);
    lua_State* b=LuaBz::detail::lua_state_factory::create_state(file_b);
    ASSERT_TRUE(a!=b);
}