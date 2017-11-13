#include "gtest/gtest.h"
#include "can_represent_value.hpp"
#include "core/lua_script.hpp"
#include <string>
using namespace ::testing;
using namespace LuaBz;
using namespace std;
class lua_scriptF:public Test
{
public:
    string main_script_dep;
    string main_script_dep2;
    string main_script;
    string script_folder_path;
    string first_dependecy_string;
    std::string second_dependency_string;
    LuaBz::lua_script script;
    void SetUp() override
    {
        script_folder_path="../src/core/tests/";
        main_script_dep=script_folder_path+"luascript_dep.lua";
        main_script_dep2=script_folder_path+"luascript_dep2.lua";
        main_script=script_folder_path+"luascript_test.lua";
        first_dependecy_string="This is lua script dependency";
        second_dependency_string="This is a second dependency";
        std::vector<string> dependencies{main_script_dep,main_script_dep2};
        script.open(main_script,dependencies);
    }
};
TEST_F(lua_scriptF,LoadingDependencyAndMainScript)
{
    ASSERT_TRUE(first_dependecy_string==script.get<string>("dependency"));
}
TEST_F(lua_scriptF,LoadingMultipleDependenciesAndMainScript)
{
    ASSERT_TRUE(first_dependecy_string==script.get<string>("dependency"));
    ASSERT_TRUE(second_dependency_string==script.get<string>("dependency2"));
}
TEST_F(lua_scriptF,GetLuaBool)
{
    ASSERT_TRUE(script.get<bool>("boolt_var"));
    ASSERT_FALSE(script.get<bool>("boolf_var"));
}
TEST_F(lua_scriptF,GetLuaString)
{
    string expected="This is some string";
    ASSERT_TRUE(expected==script.get<string>("string_var"));
}
TEST_F(lua_scriptF,GetLuaInteger)
{
     ASSERT_EQ(100,script.get<long>("integer_var"));
}
TEST_F(lua_scriptF,GetLuaNumber)
{
    ASSERT_DOUBLE_EQ(102.0351,script.get<double>("double_var"));
}
TEST_F(lua_scriptF,SetLuaBool)
{
    script.set("boolt_var",false);
    ASSERT_FALSE(script.get<bool>("boolt_var"));
}
TEST_F(lua_scriptF,SetLuaString)
{
    std::string new_value="This is the new value for the lua string";
    script.set("string_var",new_value);
    ASSERT_TRUE(new_value==script.get<string>("string_var"));    
}
TEST_F(lua_scriptF,SetLuaInteger)
{
    script.set("integer_var",666);
    ASSERT_EQ(666,script.get<long>("integer_var"));
}
TEST_F(lua_scriptF,SetLuaNumber)
{
    script.set("double_var",10.132265);
    ASSERT_DOUBLE_EQ(10.132265,script.get<double>("double_var"));
}
TEST_F(lua_scriptF,ExecuteLuaCode)
{
    string code=R"(variable=1;)"
                R"(function increment(input) )"
                R"(    input=input+1;)"
                R"(    return input;)"
                R"(end )"
                R"(variable=increment(variable) )";
    script.run(code);
    ASSERT_EQ(2,script.get<long>("variable"));
}