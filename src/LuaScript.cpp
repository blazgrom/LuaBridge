#include "LuaScript.hpp"
#include <iostream>
#include <algorithm>
namespace LuaBz
{
	//Static
	std::vector<LuaScript::LuaCF_Intermediary> LuaScript::m_registeredFunctions;
	//C-tors
	LuaScript::LuaScript(const std::string& file, bool loadStandardLib)
		:
		m_state{ luaL_newstate() },
		m_fileName{ file },
		m_open{ true }

	{
		if (loadStandardLib)
			luaL_openlibs(m_state);
		run_file(m_fileName);
	}
	LuaScript::LuaScript(const std::string& file, const std::vector<std::string>& dependencies, bool loadStandardLib)
		:
		m_state(luaL_newstate()),
		m_fileName(file),
		m_open(true)
	{
		init(dependencies, loadStandardLib);
	}
	LuaScript::LuaScript(const std::string& file, const std::string& dependency, bool loadStandardLib)
		:
		m_state(luaL_newstate()),
		m_fileName(file),
		m_open(true)
	{
		init({ dependency }, loadStandardLib);
	}
	//D-tor
	LuaScript::~LuaScript()
	{
		close();
	}
	//Public
	void LuaScript::call(const LuaFunction<void>& f) const
	{
		prepare_for_call(f.name);
		call_impl(0, f.resultCount, f.name);
	}
	std::unordered_map<std::string, std::string> LuaScript::table_info(const std::string& table) const
	{
		std::unordered_map<std::string, std::string> info;
		auto process = [&info, this]() {
			if (lua_istable(m_state, -1)) {
				auto f = [&info, this](const std::string& key) {
					info[key] = lua_typename(m_state, lua_type(m_state, -1));
				};
				iterate_table(f);
			}
		};
		if (table.find('.') == std::string::npos)
			retrieve_global(table);
		else
			retrieve_table_element(table);
		process();
		return info;
	}
	void LuaScript::open(const std::string& file)
	{
		if (!m_open)
		{
			m_state = luaL_newstate();
			run_file(file);
			m_open = true;
			m_fileName = file;
		}
	}
	void LuaScript::close() noexcept
	{
		lua_close(m_state);
		/*for ( auto index : m_localFunctions)
		{
			m_registeredFunctions.erase(m_registeredFunctions.begin() + index);
		}*/
		m_open = false;
	}
	bool LuaScript::change(const std::string& newFile) noexcept
	{
		bool result = true;
		try
		{
			lua_close(m_state);
			m_state = luaL_newstate();
			run_file(newFile);
			m_fileName = newFile;
		}
		catch (...)
		{
			run_file(m_fileName);
			result = false;
		}
		return result;
	}
	void LuaScript::run(std::string luaCode)
	{
		if (luaL_dostring(m_state, luaCode.c_str()))
		{
			error(lua_tostring(m_state, -1), true);
		}
	}
	//Private
	void LuaScript::init(const std::vector<std::string>& dependencies, bool loadStandardLib)
	{
		if (loadStandardLib)
		{
			luaL_openlibs(m_state);
		}
		for (const std::string& file : dependencies)
		{
			run_file(file);
		}
		run_file(m_fileName);
	}
	void LuaScript::run_file(const std::string& name) const
	{
		if (luaL_dofile(m_state, name.c_str()))
		{
			error(lua_tostring(m_state, -1), true);
		}
	}
	void LuaScript::push_lua_stack(const LuaTable& val) const
	{
		lua_newtable(m_state);
		for (const auto& element : val)
		{
			push_lua_stack(element.name());
			switch (element.type())
			{
			case LuaType::Boolean:
				push_lua_stack(element.boolean());
				break;
			case LuaType::Number:
				push_lua_stack(element.number());
				break;
			case LuaType::Integer:
				push_lua_stack(element.integer());
				break;
			case LuaType::Nil:
				push_lua_stack(element.nil());
				break;
			case LuaType::String:
				push_lua_stack(element.string());
				break;
			}
			lua_settable(m_state, -3); //automatically pops [key,value] 
		}
	}
	//Retrieves a lua value and pushes it on top of the stack
	void LuaScript::retrieve_lua_value(const std::string& name) const
	{
		if (name.find('.') == std::string::npos)
			retrieve_global(name);
		else
			retrieve_table_element(name);
	}
	void LuaScript::retrieve_global(const std::string& name) const
	{
		lua_getglobal(m_state, name.c_str());
		if (lua_isnoneornil(m_state, -1))
		{
			error("A variable with the name:" + name + " could not be found");
		}
	}
	void LuaScript::retrieve_table_element(const std::string& name) const
	{
		std::string tableName = name.substr(0, name.find_first_of('.')), tablefield = name.substr(name.find_first_of('.') + 1);
		retrieve_global(tableName);
		bool keepProcessing = true;
		while (keepProcessing)
		{
			auto dotPosition = tablefield.find_first_of('.');
			if (dotPosition == std::string::npos)
			{
				load_table_field(tablefield);
				keepProcessing = false;
			}
			else
			{
				std::string parent = tablefield.substr(0, dotPosition);
				tablefield = tablefield.substr(dotPosition + 1);
				load_table_field(parent);
			}
		}
	}
	void LuaScript::prepare_for_call(const std::string& name) const
	{
		load_function(name);
	}
	void LuaScript::load_function(const std::string& name) const
	{
		retrieve_lua_value(name);
		const int topElement = -1;
		if (!lua_isfunction(m_state, topElement))
		{
			error(name + "is not a function");
		}
	}
	void LuaScript::call_impl(int inputCount, int outputCount, const std::string& name) const
	{
		if (lua_pcall(m_state, inputCount, outputCount, 0) != 0)
		{
			error(name + " : " + get_pop_lua_stack<std::string>());
		}
	}
	void LuaScript::error(const std::string& message, bool popStack) const
	{
		if (popStack)
		{
			pop_lua_stack();
		}
		throw std::runtime_error(message.c_str());
	}
	void LuaScript::pop_lua_stack(int count) const
	{
		lua_pop(m_state, count);
	}
	void LuaScript::iterate_table(std::function<void(const std::string&)> predicate, bool popLastValue) const
	{
		lua_pushnil(m_state);
		unsigned int tableIndex = 1;
		while (lua_next(m_state, -2) != 0)
		{
			/*key=-2, value=-1*/
			std::string tableKey = std::to_string(tableIndex);
			if (lua_isstring(m_state, -2) && !lua_isnumber(m_state,-2))
			{
				tableKey= lua_tostring(m_state, -2);
			}
			predicate(std::string{ tableKey });
			++tableIndex;
			if (popLastValue)
				pop_lua_stack();
		}
	}
	void LuaScript::load_table_field(const std::string& field, int tableIndex) const
	{
		if (lua_istable(m_state, tableIndex))
			lua_getfield(m_state, tableIndex, field.c_str());
		else
			error("The field " + field + " could not be loaded");
	}
	LuaTable LuaScript::create_lua_table() const
	{
		LuaTable table;
		auto f = [&table, this](const std::string &key)
		{
			switch (lua_type(m_state, -1))
			{
			case LUA_TNIL:
				table.push_back(LuaValue(key, nullptr));
				break;
			case LUA_TBOOLEAN:
				table.push_back(LuaValue(key, get_pop_lua_stack<bool>()));
				break;
			case LUA_TNUMBER:
			{
				double number = lua_tonumber(m_state, -1);
				if (number == static_cast<int>(number))
					table.push_back(LuaValue(key, get_pop_lua_stack<int>()));
				else
					table.push_back(LuaValue(key, get_pop_lua_stack<double>()));
			}
			break;
			case LUA_TSTRING:
			{
				table.push_back(LuaValue(key, get_pop_lua_stack<std::string>()));
			}
			break;
			//Ignore functions and tables, right now LuaValue cannot rappresent the value of
			//a cfunction or a table
			case LUA_TFUNCTION:
			case LUA_TTABLE:
				pop_lua_stack();
				break;
			}
		};
		iterate_table(f, false);
		return table;
	}
	void LuaScript::register_function_impl(const std::string& name)
	{
		//The index at which the function is saved in the static C++ vector becomes an upvalue for the function
		int index = m_registeredFunctions.size() - 1;
		push_lua_stack(index);
		lua_CFunction lua_F = [](lua_State* state)->int 
		{
			auto functionIndex = lua_tointeger(state, lua_upvalueindex(1));//retrieve  upvalue
			auto& function = m_registeredFunctions.at(functionIndex);
			return function(state);
		};
		lua_pushcclosure(m_state, lua_F, 1);
		lua_setglobal(m_state, name.c_str());
	}
	void LuaScript::push_lua_stack(std::nullptr_t) const
	{
		lua_pushnil(m_state);
	}
	void LuaScript::push_lua_stack(std::string val) const
	{
		lua_pushlstring(m_state, val.c_str(), val.size());
	}
	void LuaScript::push_lua_stack(char val) const
	{
		std::string s{ val };
		push_lua_stack(s);
	}
	void LuaScript::push_lua_stack(bool val) const
	{
		lua_pushboolean(m_state, val);
	}
}