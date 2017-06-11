#ifndef LUABZ_LUA_FUNCTION_HPP
#define LUABZ_LUA_FUNCTION_HPP
#include <string>
namespace LuaBz
{
	template <typename... T>
	struct LuaFunction
	{
		explicit LuaFunction(const std::string& name)
			:
			name(name),
			resultCount(sizeof... (T))
		{

		}
		std::string name;
		const unsigned int resultCount;
	};
	template <>
	struct LuaFunction<void>
	{
		explicit LuaFunction(const std::string& name)
			:
			name(name),
			resultCount(0)
		{

		}
		std::string name;
		unsigned int resultCount;
	};
}
#endif // !LUABZ_LUA_FUNCTION_HPP
