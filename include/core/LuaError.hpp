#ifndef LUABZ_LUA_ERROR_HPP
#define LUABZ_LUA_ERROR_HPP
#include <stdexcept>
#include <string>
namespace LuaBz
{
	class LuaError : public std::runtime_error
	{
	public:
		explicit LuaError(const std::string& msg)
			:std::runtime_error(msg)
		{

		}
		explicit LuaError(const char* msg)
			:std::runtime_error(msg)
		{

		}
	private:
	};
}
#endif // !LUABZ_LUA_ERROR_HPP
