#include "LuaScript.hpp"
namespace Lua
{
	//Static
	std::string LuaScript::m_userFunctionInCall;
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
		for (const auto& name : m_localFunctions)
		{
			LuaScript::m_userFunctions.erase(name);
		}
	}
	//Public
	void LuaScript::call(const LuaFunction<void>& f) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		call_Impl(0, f.resultCount, f.name);
	}
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
			getGlobalVariable(table);
		else
			getTableField(table);
		process();
		return info;
	}
	void LuaScript::open(const std::string& file)
	{
		if (!m_open)
		{
			m_state = luaL_newstate();
			loadFile(file);
			m_open = true;
			m_fileName = file;
		}
	}
	void LuaScript::close() noexcept
	{
		lua_close(m_state);
		m_open = false;
	}
	bool LuaScript::change(const std::string& newFile) noexcept
	{
		bool result = true;
		try
		{
			lua_close(m_state);
			m_state = luaL_newstate();
			loadFile(newFile);
			m_fileName = newFile;
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
			error(lua_tostring(m_state, -1), true);
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
			error(lua_tostring(m_state, -1), true);
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
			getGlobalVariable(name);
		else
			getTableField(name);
		checkType();
	}
	void LuaScript::call_Impl(int inputCount, int outputCount, const std::string& name) const
	{
		if (lua_pcall(m_state, inputCount, outputCount, 0) != 0)
		{
			error(name + " : " + get_Impl<std::string>());
		}
	}
	void LuaScript::error(const std::string& message, bool popStack) const
	{
		if (popStack)
		{
			pop();
		}
		throw std::runtime_error(message.c_str());
	}
	void LuaScript::pop(int count) const
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
				pop();
		}
	}
	void LuaScript::getGlobalVariable(const std::string& name) const
	{
		lua_getglobal(m_state, name.c_str());
		if (lua_isnoneornil(m_state, -1))
		{
			error("Object with name " + name + " could not be loaded");
		}
	}
	void LuaScript::getTableField(const std::string& name) const
	{
		std::string tableName = name.substr(0, name.find_first_of('.')), tablefield = name.substr(name.find_first_of('.') + 1);
		getGlobalVariable(tableName);
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
				table.values.push_back(Lua::LuaValue(key, get_Impl<bool>()));
				break;
			case LUA_TNUMBER:
			{
				double number = lua_tonumber(m_state, -1);
				if (number == static_cast<int>(number))
					table.values.push_back(Lua::LuaValue(key, get_Impl<int>()));
				else
					table.values.push_back(Lua::LuaValue(key, get_Impl<double>()));
			}
			break;
			case LUA_TSTRING:
			{
				size_t strLength = 0;
				const char* str = lua_tolstring(m_state, -1, &strLength);
				table.values.push_back(Lua::LuaValue(key, std::string(str, strLength)));
			}
			break;
			case LUA_TFUNCTION://Ignore functions and tables
			case LUA_TTABLE:
				pop();
				break;
			}
		};
		iterateTable(f, false);
		return table;
	}
	void LuaScript::registerFunctionImpl(const std::string& name)
	{
		lua_CFunction lua_F = [](lua_State* state)->int {
			std::string name = LuaScript::m_userFunctionInCall;
			auto function = LuaScript::m_userFunctions[name];
			return function(state);
		};
		lua_pushcfunction(m_state, lua_F);
		lua_setglobal(m_state, name.c_str());
	}
	void LuaScript::push(std::nullptr_t&) const
	{
		lua_pushnil(m_state);
	}
	void LuaScript::push(bool val) const
	{
		lua_pushboolean(m_state, val);
	}
	void LuaScript::push(const std::string& val) const
	{
		lua_pushlstring(m_state, val.c_str(), val.size());
	}
	void LuaScript::push(int val) const
	{
		lua_pushinteger(m_state, val);
	}
	void LuaScript::push(double val) const
	{
		lua_pushnumber(m_state, val);
	}
	void LuaScript::push(float val) const
	{
		push(static_cast<double>(val));
	}

}