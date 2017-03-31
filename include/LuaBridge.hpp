#ifndef LUA_B_HPP
#define LUA_B_HPP
#include <map>
#include <tuple>
#include <string>
#include <functional>
#include "lua.hpp"
namespace LuaBridge
{
	struct LuaFunction
	{
		explicit LuaFunction(const std::string& name, unsigned int resultCount = 0)
			:
			name(name),
			resultCount(resultCount)
		{

		}
		std::string name;
		unsigned int resultCount;
	};
	class LuaB
	{
	public:
		explicit LuaB(const std::string& fileName)
			:
			_state(luaL_newstate()),
			_file(fileName),
			_open(true),
			_functionReturnCount(0),
			_popCount(0)
		{
			luaL_openlibs(_state);
			loadFile(_file);
		}
		LuaB(const LuaB& rhs) = delete;
		LuaB& operator=(const LuaB& rhs) = delete;
		LuaB(LuaB&& rhs) = default;
		LuaB& operator=( LuaB&& rhs) = default;
		~LuaB()
		{
			lua_close(_state);
		}
		/*
			Gets a global variable from the file loaded during the creation of the object
		*/
		template <typename T >
		T get(const std::string& name) const
		{
			if (name.find('.') == std::string::npos)
			{
				loadGlobalVariable(name);
				auto val = getValue<T>();
				popStack();
				return val;
			}
			else
			{
				try
				{
					std::string field = loadTable(name);
					loadTableField(field);
					auto result = getValue<T>();
					popStack();
					return result;
				}
				catch (std::invalid_argument& )
				{
					throw;
				}
			}
		}	
		/*
			Sets a global variable in the file loaded during the creation of the object
		*/
		template <typename T >
		bool set(const std::string& name, const T& val) const
		{
			if (name.find('.') == std::string::npos)
			{
				bool result = setGlobal(name, val);
				return result;
			}
			else
			{
				bool result = setField(name, val);
				return result;
			}
		}
		/*
			N input  0 output && N input N output
			
		*/
		template <typename... returnTypes, typename... Args>
		std::tuple<returnTypes...> call(const LuaFunction&function, Args... args) const
		{
			checkDataValidity<returnTypes ...>(function.resultCount);
			prepareForCall(function);
			int argumentsCount = loadFunctionParams(std::forward<Args>(args)...);
			callFunction(argumentsCount, _functionReturnCount,  function.name);
			return getReturnValues<returnTypes ...>();
		}		
		/*
			0 input  N output
		*/
		template <typename... T>
		std::tuple<T...> call(const LuaFunction&function) const
		{
			checkDataValidity<T...>(function.resultCount);
			prepareForCall(function);
			callFunction(0, _functionReturnCount,  function.name);
			return getReturnValues<T...>();
		}
		/*
			 0 input 0 
		*/
		void call(const LuaFunction&function) const
		{
			if (function.resultCount > 0)
			{
				throw std::exception("Function cannot have return values");
			}
			prepareForCall(function);
			callFunction(0, _functionReturnCount,  function.name);
		}
		std::map<std::string, std::string> getTableInfo(const std::string& name) const
		{
			std::map<std::string, std::string> keys;
			auto process = [&keys, this]() {
				if (lua_istable(_state, -1)) {
					auto f = [&keys, this](const std::string& key) {
						keys[key] = lua_typename(_state, lua_type(_state, -1));
					};
					iterateTable(f);
				}
			};
			if (name.find('.') == std::string::npos)
			{
				loadGlobalVariable(name);
				process();
			}
			else
			{
				std::string field= loadTable(name);
				loadTableField(field);
				process();
			}
			return keys;
		}
		void openFile(const std::string& name)
		{
			if (!_open)
			{
				_state = luaL_newstate();
				loadFile(name);
				_file = name;
			}
		}
		void closeFile()  noexcept
		{
			lua_close(_state);
			_open = false;
		}
		bool changeFile(const std::string& name)  noexcept
		{
			bool result = true;
			lua_close(_state);
			_state = luaL_newstate();
			try
			{
				loadFile(name);
				_file = name;
			}
			catch (const std::exception& e)
			{
				loadFile(_file);
				result = false;
			}
			return result;
		}
	private:
		lua_State* _state;
		std::string _file;
		bool _open;
		mutable unsigned int _functionReturnCount;//The result count of the last function that was called
		mutable unsigned int _popCount;//Number of elements that must be popped from the stack
		/*
			Loads  a lua file + standard lib and runs it
		*/
		void loadFile(const std::string&name) const
		{
			if (luaL_dofile(_state, name.c_str()))
			{
				std::string error_message = lua_tostring(_state, -1);
				generateError(error_message);
			}
		}
		void loadFunction(const std::string& name) const
		{
			auto checkType = [&name,this]() {
				if (!lua_isfunction(_state, -1))
				{
					popStack();
					throw std::invalid_argument(name + " is not a function");
				}
			};
			if (name.find('.') == std::string::npos)
			{
				loadGlobalVariable(name);
				checkType();
			}
			else
			{
				std::string field = loadTable(name);
				loadTableField(field);
				checkType();
			}
		
		}
		template <typename T, typename... Args>
		int loadFunctionParams(T value, Args... args) const
		{
			pushValue(value);
			return 1 + loadFunctionParams(args...);
		}
		template <typename T>
		int loadFunctionParams(T value) const
		{
			pushValue(value);
			return 1;
		}
		void prepareForCall(const  LuaFunction& function) const
		{
			loadFunction(function.name);
			_functionReturnCount = function.resultCount;
			_popCount = _functionReturnCount;
		}
		void callFunction(int argumentCount, int returnValuesCount, const std::string& functionName) const
		{
			if (lua_pcall(_state, argumentCount, returnValuesCount, 0) != 0) {
				generateError(functionName+" : " + getValue<std::string>());
			}
		}
		void generateError(const std::string& message) const
		{
			popStack();
			throw std::exception(message.c_str());
		}
		template<typename... T>
		void checkDataValidity(unsigned int count) const
		{
			int a = sizeof...(T);
			if (sizeof...(T) > count)
			{
				throw std::invalid_argument("Number of template parameters passed is bigger than the number of the function's return values ");
			}
			else if (sizeof...(T) < count)
			{
				throw std::invalid_argument("The number of function return values is bigger than the number of template parameters for the return values");
			}
		}
		template<typename... T>
		void checkDataValidity() const
		{
			checkDataValidity<T...>(_functionReturnCount);
		}
		template <typename T>
		T getValue(int stackIndex = -1) const
		{
			if (lua_istable(_state, -1))
			{
				std::map<std::string, std::string> data;
				auto f = [&data, this](const std::string& key) {
					if (!lua_istable(_state, -1) && !lua_isfunction(_state, -1))
					{
						data[key] = getValue<std::string>(-1);
					}
					else
					{
						data[key] = "";
					}
				};
				iterateTable(f);
				T result;
				result.luaPack(data);
				return result;
			}
			else
			{
				std::string startText = "The variable you are trying to get is not a table, thus cannot be converted to a variable of type ";
				std::string endText = typeid(T).name();
				generateError(startText + endText);
			}
		}
		template <>
		double getValue<double>(int stackIndex) const
		{
			return  static_cast<double>(lua_tonumber(_state, stackIndex));
		}
		template <>
		float getValue<float>(int stackIndex) const
		{
			return static_cast<float>(getValue<double>(stackIndex));
		}
		template <>
		int getValue<int>(int stackIndex) const
		{
			return  static_cast<int>(lua_tointeger(_state, stackIndex));
		}
		template<>
		std::string getValue<std::string>(int stackIndex) const
		{
			return static_cast<std::string>(lua_tostring(_state, stackIndex));
		}
		template <>
		bool  getValue<bool>(int stackIndex) const
		{
			return lua_toboolean(_state, stackIndex) != 0;
		}
		template <typename T>
		void pushValue(const T& val) const
		{
			createTable(val);
		}
		template <>
		void pushValue<bool>(const bool& val) const
		{
			lua_pushboolean(_state, val);
		}
		template<>
		void pushValue<std::string>(const std::string& val) const
		{
			lua_pushlstring(_state, val.c_str(), val.size());
		}
		template <>
		void pushValue<int>(const int& val) const
		{
			lua_pushinteger(_state, val);
		}
		template<>
		void pushValue<double>(const double& val) const
		{
			lua_pushnumber(_state, val);
		}
		template<>
		void pushValue<float>(const float& val) const
		{
			pushValue(static_cast<double>(val));
		}
		void popStack(int count = 1) const
		{
			lua_pop(_state, count);
		}
		template <typename... Args>
		std::tuple<Args ...> getReturnValues() const
		{
			//The order of evaluation of an initializer list in c-tor of tuple<Args...> is well defined
			return tuple<Args ...>{ getReturnValue<Args>() ... };
		}
		template <typename T>
		T getReturnValue() const
		{
			if (_functionReturnCount != 0)
			{
				auto result = getValue<T>(_functionReturnCount*-1);
				if ((--_functionReturnCount) == 0)
				{
					popStack(_popCount);
					_popCount = 0;
				}
				return result;
			}
			else
			{
				throw std::invalid_argument("You cannot get return values, because there are none");
			}
		}
		template <typename T>
		void createTable(const T& val) const
		{
			lua_newtable(_state);
			for (auto element : val.luaUnpack())
			{
				pushValue(element.first);
				pushValue(element.second);
				lua_settable(_state, -3);//automatically pops [key,value] 
			}
		}
		/*
			Iterates over a table and calls function process for every iteration passing the key(the value is on the stack with index = -1
			1-The function assumes that the tables is on top of the stack

		*/
		void iterateTable(std::function<void(const std::string&)> process) const
		{
			lua_pushnil(_state);  /* first key */
			while (lua_next(_state, -2) != 0) {
				/* uses 'key' (at index -2) and 'value' (at index -1) */
				auto key = getValue<std::string>(-2);
				process(key);
				popStack();
			}
		}
		/*
			Pushes on top of the stack the first element element from name and removes it
			from name
		*/
		std::string loadTable(std::string name)const
		{
			std::string parent = name.substr(0, name.find_first_of('.'));
			loadGlobalVariable(parent);
			name = name.substr(name.find_first_of('.') + 1);
			return name;
		}
		/*
			Pushes on top of the stack the last element from the str, where each element is 
			separed with .(dot)
			1-The function assumes that the parent table is already on top of the stack
		*/
		void loadTableField(std::string field) const
		{
			
			auto keepProcessing = true;
			while (keepProcessing)
			{
				auto dotPosition = field.find_first_of('.');
				if (dotPosition == std::string::npos)
				{
					keepProcessing = false;
					getTableField(field, -1);
				}
				else
				{
					std::string parent = field.substr(0, dotPosition);
					field = field.substr(dotPosition + 1);
					getTableField(parent, -1);
				}
			}
		}
		/*
			Load a global lua variable with name equals to name,
			if the variable could not be loaded the stack is popped and  an exception is throw signaling that the variable 
			could not be loaded
		*/
		void loadGlobalVariable(const std::string& name) const
		{
			lua_getglobal(_state, name.c_str());
			bool failed = lua_isnoneornil(_state, -1);
			if (failed)
			{
				popStack();
				throw std::invalid_argument("Object with name " + name + " could not be loaded");
			}
		}
		/*
			Pushes on top of the stack the field with name equals to name of the table at specified index,
			if the table doesnt contain a specific field an exception is thrown 
		*/
		void getTableField(const std::string& name, int index ) const
		{
			if (lua_istable(_state, index))
			{
				lua_getfield(_state, index, name.c_str());
			}
			else
			{
				popStack();
				throw std::invalid_argument("The field " + name + " could not be loaded");
			}
		}
		template <typename T>
		bool setGlobal(const std::string& name, const T& val) const
		{
			try
			{
				loadGlobalVariable(name);
				pushValue(val);
				lua_setglobal(_state, name.c_str());
				return true;
			}
			catch (const std::invalid_argument& e)
			{
				return false;
			}
		}
		template <typename T>
		bool setField(const std::string& name, const T& val)const
		{
			try
			{
				std::string field = loadTable(name);
				auto keepProcessing = true;
				while (keepProcessing)
				{
					auto dotPosition = field.find_first_of('.');
					if (dotPosition == std::string::npos)
					{
						pushValue<T>(val);
						lua_setfield(_state, -2, field.c_str());
						keepProcessing = false;
					}
					else
					{
						std::string parent = field.substr(0, dotPosition);
						field = field.substr(dotPosition + 1);
						getTableField(parent, -1);
					}
				}
				return true;
			}
			catch (const std::invalid_argument& e)
			{
				return false;
			}
		}
	};
#endif // !LUA_B_HPP
}
