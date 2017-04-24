#include "LuaScript.hpp"
namespace Lua
{
	//C-tors
	LuaScript::LuaScript(const std::string& file, bool loadStandardLib)
		:
		m_state(luaL_newstate()),
		m_fileName(file),
		m_open(true)
	{
		if (loadStandardLib)
			luaL_openlibs(m_state);
		loadFile(m_fileName);
	}
	LuaScript::LuaScript(const std::string& file, const std::vector<std::string>& dependencies, bool loadStandardLib)
		:
		m_state(luaL_newstate()),
		m_fileName(file),
		m_open(true)
	{
		initialize(dependencies, loadStandardLib);
	}
	LuaScript::LuaScript(const std::string& file, const std::string& dependency, bool loadStandardLib)
		:
		m_state(luaL_newstate()),
		m_fileName(file),
		m_open(true)
	{
		initialize({ dependency }, loadStandardLib);
	}
	//D-tor
	LuaScript::~LuaScript()
	{
		lua_close(m_state);
	}
	//Public
	std::map<std::string, std::string> LuaScript::tableInfo(const std::string& table) const
	{
		std::map<std::string, std::string> info;
		auto process = [&info, this]() {
			if (lua_istable(m_state, -1)) {
				auto f = [&info, this](const std::string& key) {
					info[key] = lua_typename(m_state, lua_type(m_state, -1));
				};
				iterateTable(f);
			}
		};
		if (table.find('.') == std::string::npos)
		{
			loadGlobal(table);
		}
		else
		{
			std::string field = loadTable(table);
			loadTableField(field);
		}
		process();
		return info;
	}
	void LuaScript::openFile(const std::string& name)
	{
		if (!m_open)
		{
			m_state = luaL_newstate();
			loadFile(name);
			m_open = true;
			m_fileName = name;
		}
	}
	void LuaScript::closeFile() noexcept
	{
		lua_close(m_state);
		m_open = false;
	}
	bool LuaScript::changeFile(const std::string& name) noexcept
	{
		bool result = true;
		lua_close(m_state);
		m_state = luaL_newstate();
		try
		{
			loadFile(name);
			m_fileName = name;
		}
		catch (...)
		{
			loadFile(m_fileName);
			result = false;
		}
		return result;
	}
	void LuaScript::run(std::string luaCode)
	{
		if (luaL_dostring(m_state, luaCode.c_str()))
		{
			error(lua_tostring(m_state, -1));
		}
	}
	//Private
	void LuaScript::initialize(const std::vector<std::string>& dependencies, bool loadStandardLib)
	{
		if (loadStandardLib)
		{
			luaL_openlibs(m_state);
		}
		for (const std::string& file : dependencies)
		{
			loadFile(file);
		}
		loadFile(m_fileName);
	}
	void LuaScript::loadFile(const std::string& name) const
	{
		if (luaL_dofile(m_state, name.c_str()))
		{
			error(lua_tostring(m_state, -1));
		}
	}
	void LuaScript::loadFunction(const std::string& name) const
	{
		auto checkType = [&name, this]() {
			if (!lua_isfunction(m_state, -1))
			{
				error(name + "is not a function");
			}
		};
		if (name.find('.') == std::string::npos)
		{
			loadGlobal(name);
		}
		else
		{
			std::string field = loadTable(name);
			loadTableField(field);
		}
		checkType();
	}
	void LuaScript::call_Impl(int inputValCount, int outputValCount, const std::string& functionName) const
	{
		if (lua_pcall(m_state, inputValCount, outputValCount, 0) != 0) {
			error(functionName + " : " +  getImpl<std::string>());
		}
	}
	void LuaScript::error(const std::string& message) const
	{
		popStack();
		throw std::runtime_error(message.c_str());
	}
	void LuaScript::popStack(int count ) const
	{
		lua_pop(m_state, count);
	}
	/*
	Iterates over a table and calls function process for every iteration passing the key(the value is on the stack with index = -1
	1-The function assumes that the tables is on top of the stack
	*/
	void LuaScript::iterateTable(std::function<void(const std::string&)> process) const
	{
		lua_pushnil(m_state);
		while (lua_next(m_state, -2) != 0)
		{
			/*key=-2, value=-1*/
			auto key =  getImpl<std::string>(-2);
			process(key);
			popStack();
		}
	}
	/*
	Pushes on top of the stack the first element  from name and return the names of the remaining fields
	*/
	std::string LuaScript::loadTable(std::string name) const
	{
		std::string parent = name.substr(0, name.find_first_of('.'));
		loadGlobal(parent);
		return name.substr(name.find_first_of('.') + 1);
	}
	/*
	Pushes on top of the stack the last element from the str, where each element is
	separed with .(dot)
	1-The function assumes that the parent table is already on top of the stack
	*/
	void LuaScript::loadTableField(std::string field) const
	{
		auto keepProcessing = true;
		while (keepProcessing)
		{
			auto dotPosition = field.find_first_of('.');
			if (dotPosition == std::string::npos)
			{
				getTableField(field, -1);
				keepProcessing = false;
			}
			else
			{
				std::string parent = field.substr(0, dotPosition);
				field = field.substr(dotPosition + 1);
				getTableField(parent, -1);
			}
		}
	}
	void LuaScript::loadGlobal(const std::string& name) const
	{
		lua_getglobal(m_state, name.c_str());
		if (lua_isnoneornil(m_state, -1))
		{
			error("Object with name " + name + " could not be loaded");
		}
	}
	/*
	Pushes on top of the stack the field with name equals to name of the table at specified index,
	if the table doesnt contain a specific field an exception is thrown
	*/
	void LuaScript::getTableField(const std::string& name, int index) const
	{
		if (lua_istable(m_state, index))
		{
			lua_getfield(m_state, index, name.c_str());
		}
		else
		{
			popStack();
			throw std::runtime_error("The field " + name + " could not be loaded");
		}
	}
	std::map<std::string, std::string > LuaScript::createValuesMap() const
	{
		std::map<std::string, std::string> data;
		auto f = [&data, this](const std::string& key) {
			if (!lua_istable(m_state, -1) && !lua_isfunction(m_state, -1))
				data[key] = getImpl<std::string>(-1);
			else
				data[key] = "";
		};
		iterateTable(f);
		return data;
	}
	Lua::LuaTable LuaScript::createLuaTable() const
	{
		Lua::LuaTable table;
		auto f = [&table, this](const std::string &key)
		{
			if (!lua_istable(m_state, -1) && !lua_isfunction(m_state, -1))
			{
				if (lua_isboolean(m_state, -1))
				{
					table.values.push_back(Lua::LuaValue(key, lua_toboolean(m_state, -1)));
				}
				else if (lua_isnil(m_state, -1))
				{
					table.values.push_back(Lua::LuaValue(key, nullptr));
				}
				else if (lua_isnumber(m_state, -1))
				{
					double number = lua_tonumber(m_state, -1);
					if (number == static_cast<int>(number))
						table.values.push_back(Lua::LuaValue(key, static_cast<int>(number)));
					else
						table.values.push_back(Lua::LuaValue(key, number));
				}
			}
		};
		iterateTable(f);
		return table;
	}

}