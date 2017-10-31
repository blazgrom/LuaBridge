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
        script_folder_path="..//src//core//tests//";
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