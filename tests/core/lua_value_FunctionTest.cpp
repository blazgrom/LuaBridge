#include <gtest/gtest.h>
#include <string>
#include "lua_script.hpp"
#include "lua_test_helpers.hpp"
#include <functional>
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
TEST_F(lua_value_Function,AssignStdFunction)
{
    auto result=false;
    std::function<int()> f=[&](){
        result=true;
        return 1;
    };
    script["stdfunction"]=f;
    int return_value=script["stdfunction"]();
    ASSERT_TRUE(return_value);
}