#ifndef LUABZ_LUA_STATE_HPP
#define LUABZ_LUA_STATE_HPP
#include <vector>
#include <algorithm>
#include <string>
#include <functional>
#include <LuaError.hpp>
#include "lua.hpp"
#include "LuaTable.hpp"
#include "can_represent_value.hpp"
namespace LuaBz
{
	class LuaStack
	{
	public:
		LuaStack(bool loadStd, const std::string& file, std::vector<std::string> dependencies = {})
			:
			m_state{ luaL_newstate() }
		{
			if (loadStd)
				luaL_openlibs(m_state);
			for (const std::string& depend : dependencies)
			{
				run_file(depend);
			}
			run_file(file);
		}
		~LuaStack()
		{
			lua_close(m_state);
		}
		void pop(int count=1) const
		{
			lua_pop(m_state, count);
		}
		void set_top_element(const std::string& name) const
		{
			if (name.find('.') == std::string::npos)
				get_global_variable(name);
			else
				get_table_element(name);
		}
		template <class T>
		T top_element(bool popTopElement = false) const
		{
			if (popTopElement)
			{
				T result = get<T>(topElement);
				pop();
				return result;
			}
			return get<T>(topElement);
		}
		template <class T>
		bool set_element(const std::string& name, T&& value) const
		{
			if (name.find('.') == std::string::npos)
				set_global_variable(name, value);
			else
				set_table_element(name, value);
			return true;
		}
		template <class T>
		T get_element(unsigned int index) const
		{
			return get<T>(index);
		}
		template <class T>
		typename std::enable_if<std::is_convertible<T, LuaTable>::value>::type push(T val) const
		{
			auto table = static_cast<LuaTable>(val);
			push(table);
		}
		void push(const LuaTable& val) const
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
				lua_settable(m_state, -3); //automatically pops the pair [key,value] 
			}
		}
		void push(lua_CFunction function) const
		{
			lua_pushcclosure(m_state, function, 1);//All C++ function are inserted as closures
		}
		void push(std::nullptr_t) const
		{
			lua_pushnil(m_state);
		}
		void push(std::string val) const
		{
			lua_pushlstring(m_state, val.c_str(), val.size());
		}
		void push(char val) const
		{
			std::string s{ val };
			push(s);
		}
		void push(bool val) const
		{
			lua_pushboolean(m_state, val);
		}
		template<typename = typename std::enable_if<Utils::can_represent_value<short, lua_Integer>()>::type>
		void push(short val) const
		{
			lua_pushinteger(m_state, val);
		}
		template<typename = typename std::enable_if<Utils::can_represent_value<unsigned short, lua_Integer>()>::type>
		void push(unsigned short val) const
		{
			lua_pushinteger(m_state, val);
		}
		template <typename = typename std::enable_if<Utils::can_represent_value<int, lua_Integer>()>::type>
		void push(int val) const
		{
			lua_pushinteger(m_state, val);
		}
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned int, lua_Integer>()>::type>
		void push(unsigned int val) const
		{
			lua_pushinteger(m_state, val);
		}
		template <typename = typename std::enable_if<Utils::can_represent_value<long, lua_Integer>()>::type>
		void push(long val) const
		{
			lua_pushinteger(m_state, val);
		}
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long, lua_Integer>()>::type>
		void push(unsigned long val) const
		{
			lua_pushinteger(m_state, val);
		}
		template <typename = typename std::enable_if<Utils::can_represent_value<long long, lua_Integer>()>::type>
		void push(long long val) const
		{
			lua_pushinteger(m_state, val);
		}
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long long, lua_Integer>()>::type>
		void push(unsigned long long val) const
		{
			lua_pushinteger(m_state, val);
		}
		template <typename = typename std::enable_if < Utils::can_represent_value <float, lua_Number>()>::type >
		void push(float val) const
		{
			lua_pushnumber(m_state, val);
		}
		template <typename = typename std::enable_if<Utils::can_represent_value<double, lua_Number>()>::type>
		void push(double val) const
		{
			lua_pushnumber(m_state, val);
		}
		void call_function(int inputCount, int outputCount) const
		{
			if (lua_pcall(m_state, inputCount, outputCount, 0) != 0)
			{
				bool popTopElement = true;
				throw LuaError(top_element<std::string>(popTopElement));
			}
		}
		bool is_function(int index = -1) const
		{
			return lua_isfunction(m_state, index);
		}
		int size() const
		{
			return lua_gettop(m_state);
		}
		void execute(const std::string& code) const
		{
			if (luaL_dostring(m_state, code.c_str()))
			{
				throw LuaError(lua_tostring(m_state, topElement));
			}
		}
		void destroy()
		{
			lua_close(m_state);
		}
		void create(const std::string& file)
		{
			m_state=luaL_newstate();
			run_file(file);
		}
	private:
		lua_State* m_state;
		static const int topElement = -1;
		template <class T>
		bool set_global_variable(const std::string& name, T&& value) const
		{
			try
			{
				//TODO:The try catch and change the return type of set_global_variable
				//and set_table_element
				push(value);
				lua_setglobal(m_state, name.c_str());
				return true;
			}
			catch (const std::runtime_error&)
			{
				return false;
			}
		}
		template <class T>
		bool set_table_element(const std::string& name, const T& val) const
		{
			try
			{
				std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
				get_global_variable(tableName);//
				auto keepProcessing = true;
				while (keepProcessing)
				{
					auto dotPosition = tableField.find_first_of('.');
					if (dotPosition == std::string::npos)
					{
						push(val);
						lua_setfield(m_state, -2, tableField.c_str());
						keepProcessing = false;
					}
					else
					{
						std::string parent = tableField.substr(0, dotPosition);
						tableField = tableField.substr(dotPosition + 1);
						load_table_field(parent);
					}
				}
				return true;
			}
			catch (const std::runtime_error&)
			{
				return false;
			}
		}
		void run_file(const std::string& name) const
		{
			if (luaL_dofile(m_state, name.c_str()))
			{
				throw LuaError(lua_tostring(m_state, topElement));
			}
		}
		void get_global_variable(const std::string& name) const
		{
			lua_getglobal(m_state, name.c_str());
			if (lua_isnoneornil(m_state, topElement))
			{
				throw LuaError("A variable with the name:" + name + " could not be found");
			}
		}
		void get_table_element(const std::string& name) const
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
		void load_table_field(const std::string& name) const
		{
			if (!lua_istable(m_state, topElement))
			{
				throw LuaError("The field " + name + " could not be loaded");
			}
			lua_getfield(m_state, topElement, name.c_str());
		}
		template<class T>
		T get_lua_number(int index, std::true_type) const
		{
			T result = static_cast<T>(lua_tonumber(m_state, index));
			return result;
		}
		template <class T>
		T get_lua_number(int index, std::false_type) const
		{
			assert(false);
			return T{};
		}
		template <class T>
		T get_lua_integer(int index, std::true_type) const
		{
			T result = static_cast<T>(lua_tointeger(m_state, index));
			return result;
		}
		template <class T>
		T get_lua_integer(int index, std::false_type) const
		{
			assert(false);
			return T{};
		}
		template <class T>
		T get(int index) const
		{
			static_assert(std::is_constructible<T, LuaTable>::value, "Type cannot be constructed from a LuaTable");
			return T{ get<LuaTable>(index)};
		}
		template<>
		LuaTable get<LuaTable>(int index) const
		{
			if (!lua_istable(m_state, index))
			{
				throw LuaError("The type you are trying to retrieve cannot be constructed with a LuaTable");
			}
			return  create_lua_table();
		}
		template<>
		std::string get<std::string>(int index) const
		{
			size_t strLength = 0;
			const char* str = lua_tolstring(m_state, index, &strLength);
			std::string r{ str, strLength };
			return r;
		}
		template <>
		char get<char>(int index) const
		{
			auto str = get<std::string>(index);
			if (str.length() > 1)
			{
				throw LuaError("The value you are trying to retrieve has more than one characters");
			}
			return str[0];
		}
		template <>
		bool get<bool>(int index) const
		{
			return lua_toboolean(m_state, index) != 0;
		}
		template <>
		short get<short>(int index) const
		{
			return get_lua_integer<short>(index, Utils::Can_represent_value<lua_Integer, short>{});
		}
		template <>
		unsigned short get<unsigned short>(int index) const
		{
			return get_lua_integer<unsigned short>(index, Utils::Can_represent_value<lua_Integer, unsigned short>{});
		}
		template <>
		int get<int>(int index) const
		{
			return get_lua_integer<int>(index, Utils::Can_represent_value<lua_Integer, int>{});
		}
		template <>
		unsigned int get<unsigned int>(int index) const
		{
			return get_lua_integer<unsigned int>(index, Utils::Can_represent_value<lua_Integer, unsigned int>{});
		}
		template <>
		long get<long>(int index) const
		{
			return get_lua_integer<long>(index, Utils::Can_represent_value<lua_Integer, long>{});
		}
		template <>
		unsigned long get<unsigned long>(int index) const
		{
			return get_lua_integer<unsigned long>(index, Utils::Can_represent_value<lua_Integer, unsigned long>{});
		}
		template <>
		long long get<long long>(int index) const
		{
			return get_lua_integer<long long>(index, Utils::Can_represent_value<lua_Integer, long long>{});
		}
		template <>
		unsigned long long get<unsigned long long>(int index) const
		{
			return get_lua_integer<unsigned long long>(index, Utils::Can_represent_value<lua_Integer, unsigned long long>{});
		}
		template <>
		double  get<double>(int index) const
		{
			return get_lua_number<double>(index, Utils::Can_represent_value<lua_Number, double>{});
		}
		template <>
		float  get<float>(int index) const
		{
			return get_lua_number<float>(index, Utils::Can_represent_value<lua_Number, float>{});
		}
		LuaTable create_lua_table() const
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
				//Add support for LUATFUNCTION and LUATTABLE, for now these types are ignored
				case LUA_TFUNCTION:
				case LUA_TTABLE:
					break;
				}
			};
			iterate_table(callback);
			return table;
		}
		void iterate_table(std::function<void(const std::string&)> callback) const
		{
			lua_pushnil(m_state);//Initial key
			unsigned int tableIndex = 1;
			const int keyIndex = -2;
			while (lua_next(m_state, keyIndex) != 0)
			{
				/*key=-2, value=-1*/
				std::string tableKey = std::to_string(tableIndex++);
				if (lua_isstring(m_state, keyIndex) && !lua_isnumber(m_state, keyIndex))
				{
					tableKey = lua_tostring(m_state, keyIndex);
				}
				callback(std::string{ tableKey });
				pop();
			}
		}
		
	};
}
#endif // !LUABZ_LUA_STATE_HPP