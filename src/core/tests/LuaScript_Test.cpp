#include "gtest/gtest.h"
#include "core/LuaScript.hpp"
#include <string>
using namespace ::testing;
using namespace LuaBz;
using namespace std;
class LuaScriptF:public Test
{
public:
    string main_script_dep;
    string main_script_dep2;
    string main_script;
    string script_folder_path;
    void SetUp() override
    {
        script_folder_path="../src/core/tests/";
        main_script_dep=script_folder_path+"luascript_dep.lua";
        main_script_dep2=script_folder_path+"luascript_dep2.lua";
        main_script=script_folder_path+"luascript_test.lua";
    }
};
TEST_F(LuaScriptF,LoadingDependencyAndMainScript)
{
    std::string expected="This is lua script dependency";
    LuaScript script(main_script,main_script_dep);
    ASSERT_TRUE(expected==script.get<string>("dependency"));
}
TEST_F(LuaScriptF,LoadingMultipleDependenciesAndMainScript)
{
    std::string first_dep="This is lua script dependency";
    std::string second_dep="This is a second dependency";
    std::vector<string> dependencies{main_script_dep,main_script_dep2};
    LuaScript script(main_script,dependencies);
    ASSERT_TRUE(first_dep==script.get<string>("dependency"));
    ASSERT_TRUE(second_dep==script.get<string>("dependency2"));
}
TEST_F(LuaScriptF,GetVariableBool)
{
    LuaScript script(main_script);
    ASSERT_TRUE(script.get<bool>("boolt_var"));
    ASSERT_FALSE(script.get<bool>("boolf_var"));
}
TEST_F(LuaScriptF,GetVariableString)
{
    LuaScript script(main_script);
    string expected="This is some string";
    ASSERT_TRUE(expected==script.get<string>("string_var"));
}
TEST_F(LuaScriptF,GetVariableInteger)
{
    LuaScript script(main_script);
    auto a=script.get<int>("integer_var");
    ASSERT_EQ(1,script.get<int>("integer_var"));
}
// TEST_F(LuaScriptF,GetVariableDouble)
// {
//     LuaScript script(main_script);
//     ASSERT_DOUBLE_EQ(102.0351,script.get<double>("double_var"));
// }
// TEST_F(LuaScriptF,GetVariableFloat)
// {
//     LuaScript script(main_script);
//     ASSERT_FLOAT_EQ(88.15836,script.get<float>("float_var"));
// }