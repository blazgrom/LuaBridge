#include "core/LuaScript.hpp"
#include "core/lua_error.hpp"
#include <algorithm>
namespace LuaBz
{
	//Static
	std::vector<LuaScript::LuaCF_Intermediary> LuaScript::m_registeredFunctions;
	//C-tors
	LuaScript::LuaScript()
		:
		m_stack{},
		m_fileName{""},
		m_open{false}
	{

	}
	LuaScript::LuaScript(const std::string& file, bool loadStandardLib)
		:
		m_stack{ loadStandardLib,file },
		m_fileName{ file },
		m_open{ true }
	{
	}
	LuaScript::LuaScript(const std::string& file, const std::vector<std::string>& dependencies, bool loadStandardLib)
		:
		m_stack{ loadStandardLib,file,dependencies },
		m_fileName{ file },
		m_open{ true }
	{
	}
	LuaScript::LuaScript(const std::string& file, const std::string& dependency, bool loadStandardLib)
		:
		m_stack{ loadStandardLib,file,{ dependency } },
		m_fileName{ file },
		m_open{ true }
	{
	}
	
	//Public
	void LuaScript::call(const LuaFunction<void>& f) const
	{
		load_function(f.name());
		m_stack.call_function(0, f.result_count());
	}
	void LuaScript::open(const std::string& file,std::vector<std::string> dependencies,bool loadStandardLib)
	{
		if (!m_open)
		{
			m_stack.create(file,dependencies,loadStandardLib);
			m_open = true;
			m_fileName = file;
		}
	}
	void LuaScript::close() noexcept
	{
		m_stack.destroy();
		m_open = false;
	}
	bool LuaScript::change(const std::string& newFile) noexcept
	{
		bool result = true;
		try
		{
			m_stack.destroy();
			m_stack.create(newFile);
			m_fileName = newFile;
		}
		catch (...)
		{
			m_stack.destroy();
			m_stack.create(m_fileName);
			result = false;
		}
		return result;
	}
	void LuaScript::run(std::string luaCode)
	{
		m_stack.execute(luaCode);
	}
	void LuaScript::load_function(const std::string& name) const
	{
		m_stack.set_top_element(name);
		const int topElement = -1;
		if (!m_stack.is_function(topElement))
		{
			throw lua_error(name + "is not a function");
		}
	}
	void LuaScript::register_function_impl(const std::string& name)
	{
		//Note:
		//The index at which the function is saved in the static C++ vector becomes an upvalue for the function
		int index = m_registeredFunctions.size() - 1;
		m_stack.push(index);
		lua_CFunction lua_F = [](lua_State* functionState)->int 
		{
			auto functionIndex = lua_tointeger(functionState, lua_upvalueindex(1));//retrieve  upvalue
			auto& function = m_registeredFunctions.at(functionIndex);
			return function(functionState);
		};
		m_stack.set_element(name, lua_F);
	}
}