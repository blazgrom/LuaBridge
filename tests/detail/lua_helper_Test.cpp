#include "luabz.hpp"
#include "../core/lua_test_helpers.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(state_Test, CallingFactoryTwiceOnTheSameFileReturnSameLuaState)
{
    std::string lua_file = construct_script_path("luascript_test.lua");
    lua_State* a = luabz::state::get(lua_file);
    lua_State* b = luabz::state::get(lua_file);
    ASSERT_TRUE(a == b);
    luabz::state::close(lua_file);
}
TEST(state_Test, TwoDifferentFileHaveDifferentStates)
{
    std::string file_a = construct_script_path("luascript_test.lua");
    std::string file_b = construct_script_path("luascript_test2.lua");
    lua_State* a = luabz::state::get(file_a);
    lua_State* b = luabz::state::get(file_b);
    ASSERT_TRUE(a != b);
    luabz::state::close(file_a);
    luabz::state::close(file_b);
}