#include "lua_test_helpers.hpp"
#include "luabz.hpp"
#include <gtest/gtest.h>
#include <string>
class script_UserType : public ::testing::Test
{
  public:
    luabz::script script{construct_script_path("luascript_test.lua")};
    void TearDown() override { script.close(); }
};

TEST_F(script_UserType, Test) {}