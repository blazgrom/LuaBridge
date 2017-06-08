#ifndef LUA_HELPERS_HPP
#define LUA_HELPERS_HPP
#include <string>
#include <vector>
#include "LuaValue.hpp"
namespace LuaBz
{
	
	//Homogeneous table
	template<class T>
	struct LuaTable1
	{

	};
	//Heterogeneous table
	template<>
	class LuaTable1<LuaValue>
	{
		LuaTable1(std::vector<LuaValue>::size_type count,const LuaValue& value)
		{
			
		}
	private:
		std::vector<LuaValue> values;
	};
	struct LuaTable
	{
		std::vector<LuaValue> values;
	};
}
#endif // !LUA_HELPERS_HPP
