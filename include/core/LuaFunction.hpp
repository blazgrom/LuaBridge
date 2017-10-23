#ifndef LUABZ_LUA_FUNCTION_HPP
#define LUABZ_LUA_FUNCTION_HPP
#include <string>
namespace LuaBz
{
	
	template <typename... T>
	class LuaFunction
	{
	public:
		explicit LuaFunction(const std::string& name)
			:
			m_name(name),
			m_resultCount(sizeof... (T))
		{

		}
		const std::string& name()
		{
			return m_name;
		}
		std::string name() const
		{
			return m_name;
		}
		const unsigned int result_count() const
		{
			return m_resultCount;
		}
	private:
		std::string m_name;
		const unsigned int m_resultCount;
	};
	template <>
	class LuaFunction<void>
	{
	public:
		explicit LuaFunction(const std::string& name)
			:
			m_name(name),
			m_resultCount(0)
		{

		}
		const std::string& name()
		{
			return m_name;
		}
		std::string name() const
		{
			return m_name;
		}
		const unsigned int result_count() const
		{
			return m_resultCount;
		}
	private:
		std::string m_name;
		unsigned int m_resultCount;
	};
}
#endif // !LUABZ_LUA_FUNCTION_HPP
