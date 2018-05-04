#include "luabz.hpp"
#include <benchmark/benchmark.h>
#include <vector>
std::string lua_file = LUABZ_PROJECT_FOLDER_PATH "/benchmark/luascript.lua";
static void BM_ReadWriteGlobalLuaVariable(benchmark::State& state)
{
    using namespace luabz;
    script script(lua_file);
    int a = 1;
    for (auto _ : state) {
        script["value"] = a;
        int v = script["value"];
        if (v != a) {
            throw std::logic_error("");
        }
    }
}
BENCHMARK(BM_ReadWriteGlobalLuaVariable);

static void BM_ReadWriteTableFieldValue(benchmark::State& state)
{
    using namespace luabz;
    script script(lua_file);
    int a = 1;
    for (auto _ : state) {
        script["Position"]["x"] = a;
        int v = script["Position"]["x"];
        if (v != a) {
            throw std::logic_error("");
        }
    }
}
BENCHMARK(BM_ReadWriteTableFieldValue);

int function()
{
    return 111;
}
static void BM_CallNativeFunction(benchmark::State& state)
{
    using namespace luabz;
    script script(lua_file);
    int a = 111;
    script["value"].assign(function);
    for (auto _ : state) {
        int v = script["value"]();
        if (v != a) {
            throw std::logic_error("");
        }
    }
}
// BENCHMARK(BM_CallNativeFunction);

static void BM_CallCapturelessLambda(benchmark::State& state)
{
    using namespace luabz;
    script script(lua_file);
    int a = 111;
    script["lambda"].assign([]() mutable -> int { return 111; });
    for (auto _ : state) {
        int v = script["lambda"]();
        if (v != a) {
            throw std::logic_error("");
        }
    }
}
// BENCHMARK(BM_CallCapturelessLambda);
static void BM_CallingLuaFunction(benchmark::State& state)
{
    using namespace luabz;
    script script(lua_file);
    std::string text = "testtext";
    for (auto _ : state) {
        std::string result = script["lua_function"](text);
        if (result != text) {
            throw std::logic_error("");
        }
    }
}
BENCHMARK(BM_CallingLuaFunction);

BENCHMARK_MAIN();
