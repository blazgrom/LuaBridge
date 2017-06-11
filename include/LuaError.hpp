#ifndef LUABZ_LUA_ERROR_HPP
#define LUABZ_LUA_ERROR_HPP
#include <stdexcept>
#include <string>
namespace LuaBz
{
	class LuaError : public std::runtime_error
	{
	public:
		LuaError(const std::string& msg)
			:std::runtime_error(msg)
		{

		}
		LuaError(const char* msg)
			:std::runtime_error(msg)
		{

		}
		const char* what() const
		{
			return m_msg.c_str();
		}
	private:
		std::string m_msg;
	};
}
#endif // !LUABZ_LUA_ERROR_HPP
