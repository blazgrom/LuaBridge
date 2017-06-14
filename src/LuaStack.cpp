#include "LuaStack.hpp"
namespace LuaBz
{
	void LuaStack::pop(int count) const
	{
		lua_pop(m_state, count);
	}
	void LuaStack::set_top_element(const std::string& name) const
	{
		if (name.find('.') == std::string::npos)
			get_global_variable(name);
		else
			get_table_element(name);
	}
	void LuaStack::push(const LuaTable& val) const
	{
		lua_newtable(m_state);
		for (const auto& element : val)
		{
			push(element.name());
			switch (element.type())
			{
			case LuaType::Boolean:
				push(element.value<bool>());
				break;
			case LuaType::Number:
				push(element.value<double>());
				break;
			case LuaType::Integer:
				push(element.value<int>());
				break;
			case LuaType::Nil:
				push(element.value<std::nullptr_t>());
				break;
			case LuaType::String:
				push(element.value<std::string>());
				break;
			}
			lua_settable(m_state, -3); //Note: automatically pops the pair [key,value] 
		}
	}
	void LuaStack::push(lua_CFunction function) const
	{
		//Note:
		//All C++ function are inserted as closures with one upvalue, this upvalue contains the index in the static element containing all the 
		//user defined functions
		const int upvalue_count = 1;
		lua_pushcclosure(m_state, function, upvalue_count);
	}
	void LuaStack::push(std::nullptr_t) const
	{
		lua_pushnil(m_state);
	}
	void LuaStack::push(std::string val) const
	{
		lua_pushlstring(m_state, val.c_str(), val.size());
	}
	void LuaStack::push(char val) const
	{
		std::string s{ val };
		push(s);
	}
	void LuaStack::push(bool val) const
	{
		lua_pushboolean(m_state, val);
	}
	void LuaStack::call_function(int inputCount, int outputCount) const
	{
		if (lua_pcall(m_state, inputCount, outputCount, 0) != 0)
		{
			bool popTopElement = true;
			throw LuaError(top_element<std::string>(popTopElement));
		}
	}
	bool LuaStack::is_function(int index ) const
	{
		return lua_isfunction(m_state, index);
	}
	int LuaStack::size() const
	{
		return lua_gettop(m_state);
	}
	void LuaStack::execute(const std::string& code) const
	{
		if (luaL_dostring(m_state, code.c_str()))
		{
			throw LuaError(lua_tostring(m_state, topElement));
		}
	}
	void LuaStack::destroy()
	{
		lua_close(m_state);
	}
	void LuaStack::create(const std::string& file)
	{
		m_state = luaL_newstate();
		run_file(file);
	}
	void LuaStack::run_file(const std::string& name) const
	{
		if (luaL_dofile(m_state, name.c_str()))
		{
			throw LuaError(lua_tostring(m_state, topElement));
		}
	}
	void LuaStack::get_global_variable(const std::string& name) const
	{
		lua_getglobal(m_state, name.c_str());
		if (lua_isnoneornil(m_state, topElement))
		{
			throw LuaError("A variable with the name:" + name + " could not be found");
		}
	}
	void LuaStack::get_table_element(const std::string& name) const
	{
		std::string tableName = name.substr(0, name.find_first_of('.')), tablefield = name.substr(name.find_first_of('.') + 1);
		get_global_variable(tableName);
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
	void LuaStack::load_table_field(const std::string& name) const
	{
		if (!lua_istable(m_state, topElement))
		{
			throw LuaError("The field " + name + " could not be loaded");
		}
		lua_getfield(m_state, topElement, name.c_str());
	}
	LuaTable LuaStack::create_lua_table() const
	{
		LuaTable table;
		auto callback = [&table, this](const std::string &key)
		{
			switch (lua_type(m_state, topElement))
			{
			case LUA_TNIL:
				table.push_back(LuaValue(key, nullptr));
				break;
			case LUA_TBOOLEAN:
				table.push_back(LuaValue(key, top_element<bool>()));
				break;
			case LUA_TNUMBER:
			{
				double number = lua_tonumber(m_state, topElement);
				if (number == static_cast<int>(number))
					table.push_back(LuaValue(key, top_element<int>()));
				else
					table.push_back(LuaValue(key, top_element<double>()));
			}
			break;
			case LUA_TSTRING:
			{
				table.push_back(LuaValue(key, top_element<std::string>()));
			}
			break;
			//TODO:
			//Add support for LUA_TFUNCTION and LUA_TTABLE
			case LUA_TFUNCTION:
			case LUA_TTABLE:
				break;
			}
		};
		iterate_table(callback);
		return table;
	}
	void LuaStack::iterate_table(std::function<void(const std::string&)> callback) const
	{
		lua_pushnil(m_state);//Initial key
		const int keyIndex = -2;
		while (lua_next(m_state, keyIndex) != 0)
		{
			/*key=-2, value=-1*/
			std::string tableKey{};
			if (lua_isstring(m_state, keyIndex) && !lua_isnumber(m_state, keyIndex))
			{
				tableKey = lua_tostring(m_state, keyIndex);
			}
			else if (lua_isnumber(m_state, keyIndex))
			{
				tableKey = std::to_string(get<int>(keyIndex));
			}
			callback(tableKey);
			pop();
		}
	}
}