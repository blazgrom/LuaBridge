#include "LuaScript.hpp"
namespace Lua
{
	//Static
	std::string LuaScript::m_userF;
	std::unordered_map<std::string, std::function<int(lua_State*)>> LuaScript::m_userFunctions;
	//C-tors
	LuaScript::LuaScript(const std::string& file, bool loadStandardLib)
		:
		m_state{ luaL_newstate() },
		m_fileName{ file },
		m_open{ true }

	{
		if (loadStandardLib)
			luaL_openlibs(m_state);
		runFile(m_fileName);
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
		prepareForCall(f.name);
		callImpl(0, f.resultCount, f.name);
	}
	std::unordered_map<std::string, std::string> LuaScript::table_info(const std::string& table) const
	{
		std::unordered_map<std::string, std::string> info;
		auto process = [&info, this]() {
			if (lua_istable(m_state, -1)) {
				auto f = [&info, this](const std::string& key) {
					info[key] = lua_typename(m_state, lua_type(m_state, -1));
				};
				iterateTable(f);
			}
		};
		if (table.find('.') == std::string::npos)
			retrieveGlobal(table);
		else
			retrieveTableElement(table);
		process();
		return info;
	}
	void LuaScript::open(const std::string& file)
	{
		if (!m_open)
		{
			m_state = luaL_newstate();
			runFile(file);
			m_open = true;
			m_fileName = file;
		}
	}
	void LuaScript::close() noexcept
	{
		lua_close(m_state);
		for (const auto& name : m_localFunctions)
		{
			LuaScript::m_userFunctions.erase(name);
		}
		m_open = false;
	}
	bool LuaScript::change(const std::string& newFile) noexcept
	{
		bool result = true;
		try
		{
			lua_close(m_state);
			m_state = luaL_newstate();
			runFile(newFile);
			m_fileName = newFile;
		}
		catch (...)
		{
			runFile(m_fileName);
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
			runFile(file);
		}
		runFile(m_fileName);
	}
	void LuaScript::runFile(const std::string& name) const
	{
		if (luaL_dofile(m_state, name.c_str()))
		{
			error(lua_tostring(m_state, -1), true);
		}
	}
	//Retrieves a lua value and pushes it on top of the stack
	void LuaScript::retrieveLuaValue(const std::string& name) const
	{
		if (name.find('.') == std::string::npos)
			retrieveGlobal(name);
		else
			retrieveTableElement(name);
	}
	void LuaScript::retrieveGlobal(const std::string& name) const
	{
		lua_getglobal(m_state, name.c_str());
		if (lua_isnoneornil(m_state, -1))
		{
			error("A variable with the name:" + name + " could not be found");
		}
	}
	void LuaScript::retrieveTableElement(const std::string& name) const
	{
		std::string tableName = name.substr(0, name.find_first_of('.')), tablefield = name.substr(name.find_first_of('.') + 1);
		retrieveGlobal(tableName);
		bool keepProcessing = true;
		while (keepProcessing)
		{
			auto dotPosition = tablefield.find_first_of('.');
			if (dotPosition == std::string::npos)
			{
				loadTableField(tablefield);
				keepProcessing = false;
			}
			else
			{
				std::string parent = tablefield.substr(0, dotPosition);
				tablefield = tablefield.substr(dotPosition + 1);
				loadTableField(parent);
			}
		}
	}
	void LuaScript::prepareForCall(const std::string& name) const
	{
		setUserFunctionToRun(name);
		loadFunction(name);
	}
	void LuaScript::loadFunction(const std::string& name) const
	{
		retrieveLuaValue(name);
		if (!lua_isfunction(m_state, -1))
		{
			error(name + "is not a function");
		}
	}
	void LuaScript::setUserFunctionToRun(const std::string& func) const
	{
		if (LuaScript::m_userFunctions.find(func) != LuaScript::m_userFunctions.end() && std::find(m_localFunctions.cbegin(), m_localFunctions.cend(),func)!= m_localFunctions.cend())
		{
			LuaScript::m_userF = func;
		}
	}
	void LuaScript::callImpl(int inputCount, int outputCount, const std::string& name) const
	{
		if (lua_pcall(m_state, inputCount, outputCount, 0) != 0)
		{
			error(name + " : " + topLuaStack<std::string>());
		}
	}
	void LuaScript::error(const std::string& message, bool popStack) const
	{
		if (popStack)
		{
			popLuaStack();
		}
		throw std::runtime_error(message.c_str());
	}
	void LuaScript::popLuaStack(int count) const
	{
		lua_pop(m_state, count);
	}
	void LuaScript::iterateTable(std::function<void(const std::string&)> predicate, bool popLastValue) const
	{
		lua_pushnil(m_state);
		while (lua_next(m_state, -2) != 0)
		{
			/*key=-2, value=-1*/
			auto str = lua_tostring(m_state, -2);
			predicate(std::string{ str });
			if (popLastValue)
				popLuaStack();
		}
	}
	void LuaScript::loadTableField(const std::string& field, int tableIndex) const
	{
		if (lua_istable(m_state, tableIndex))
			lua_getfield(m_state, tableIndex, field.c_str());
		else
			error("The field " + field + " could not be loaded");
	}
	LuaTable LuaScript::createLuaTable() const
	{
		Lua::LuaTable table;
		auto f = [&table, this](const std::string &key)
		{
			switch (lua_type(m_state, -1))
			{
			case LUA_TNIL:
				table.values.push_back(Lua::LuaValue(key, nullptr));
				break;
			case LUA_TBOOLEAN:
				table.values.push_back(Lua::LuaValue(key, topLuaStack<bool>()));
				break;
			case LUA_TNUMBER:
			{
				double number = lua_tonumber(m_state, -1);
				if (number == static_cast<int>(number))
					table.values.push_back(Lua::LuaValue(key, topLuaStack<int>()));
				else
					table.values.push_back(Lua::LuaValue(key, topLuaStack<double>()));
			}
			break;
			case LUA_TSTRING:
			{
				size_t strLength = 0;
				const char* str = lua_tolstring(m_state, -1, &strLength);
				table.values.push_back(Lua::LuaValue(key, std::string(str, strLength)));
			}
			break;
			//Ignore functions and tables, right now LuaValue cannot rappresent the value of
			//a cfunction or a table
			case LUA_TFUNCTION:
			case LUA_TTABLE:
				popLuaStack();
				break;
			}
		};
		iterateTable(f, false);
		return table;
	}
	void LuaScript::registerFunctionImpl(const std::string& name)
	{
		lua_CFunction lua_F = [](lua_State* state)->int 
		{
			auto& function = LuaScript::m_userFunctions[LuaScript::m_userF];
			return function(state);
		};
		lua_pushcfunction(m_state, lua_F);
		lua_setglobal(m_state, name.c_str());
	}
	
	void LuaScript::pushLuaStack(std::nullptr_t) const
	{
		lua_pushnil(m_state);
	}
	void LuaScript::pushLuaStack(std::string val) const
	{
		lua_pushlstring(m_state, val.c_str(), val.size());
	}
	void LuaScript::pushLuaStack(char val) const
	{
		std::string s{ val };
		pushLuaStack(s);
	}
	void LuaScript::pushLuaStack(bool val) const
	{
		lua_pushboolean(m_state, val);
	}
}