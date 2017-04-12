#ifndef LUA_B_HPP
#define LUA_B_HPP
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <functional>
#include "lua.hpp"
namespace LuaWrapper
{
	template <typename... T>
	struct LuaFunction
	{
		explicit LuaFunction(const std::string& name)
			:
			name(name),
			resultCount(sizeof... (T))
		{

		}
		std::string name;
		unsigned int resultCount;
	};
	template <>
	struct LuaFunction<void>
	{
		explicit LuaFunction(const std::string& name)
			:
			name(name),
			resultCount(0)
		{

		}
		std::string name;
		unsigned int resultCount;
	};
	class LuaBz
	{
	public:
		LuaBz(const std::string& fileName,const std::vector<std::string>& preloadedFiles)
			:
			_state(luaL_newstate()),
			_file(fileName)
		{
			luaL_openlibs(_state);
			for (const std::string& file:preloadedFiles)
			{
				luaL_dofile(_state, file.c_str());
			}
			loadFile(_file);
			_open = true;
		}
		LuaBz(const std::string& fileName, const std::string& preloadedFile)
			:
			_state(luaL_newstate()),
			_file(fileName)
		{
			luaL_openlibs(_state);
			luaL_dofile(_state, preloadedFile.c_str());
			loadFile(_file);
			_open = true;
		}
		explicit LuaBz(const std::string& fileName)
			:
			_state(luaL_newstate()),
			_file(fileName)
		{
			luaL_openlibs(_state);
			loadFile(_file);
			_open = true;
		}
		LuaBz(const LuaBz& rhs) = delete;
		LuaBz& operator=(const LuaBz& rhs) = delete;
		LuaBz(LuaBz&& rhs) = default;
		LuaBz& operator=(LuaBz&& rhs) = default;
		~LuaBz()
		{
			lua_close(_state);
		}
		/*
			Gets a global variable from the file loaded during the creation of the object
		*/
		template <typename T>
		T get(const std::string& name) const
		{
			if (name.find('.') == std::string::npos)
			{
				loadGlobalVariable(name);
			}
			else
			{
				std::string field = loadTable(name);
				loadTableField(field);
			}
			auto result = getValue<T>();
			popStack();
			return result;
		}	
		/*
			Sets a global variable in the file loaded during the creation of the object
		*/
		template <typename T>
		bool set(const std::string& name, const T& val) const
		{
			if (name.find('.') == std::string::npos)
			{
				return setGlobal(name, val);
			}
			else
			{
				return setField(name, val);
			}
		}
		/*
			N input N output
		*/
		template <typename... returnTypes, typename... Args>
		std::tuple<returnTypes...> call(const LuaFunction<returnTypes...>& function, Args&&... args) const
		{
			checkDataValidity<returnTypes ...>(function.resultCount);
			loadFunction(function.name);
			int argumentsCount = loadFunctionParams(std::forward<Args>(args)...);
			callFunction(argumentsCount, function.resultCount,  function.name);
			return getReturnValues<returnTypes ...>(function.resultCount);
		}
		/*
			N input  0 output
		*/
		template <typename...Args>
		void call(const LuaFunction<void>& function, Args&&... args) const
		{
			loadFunction(function.name);
			int argumentsCount = loadFunctionParams(std::forward<Args>(args)...);
			callFunction(argumentsCount, function.resultCount, function.name);
		}
		/*
			0 input  N output
		*/
		template <typename... T>
		std::tuple<T...> call(const LuaFunction<T...>& function) const
		{
			checkDataValidity<T...>(function.resultCount);
			loadFunction(function.name);
			callFunction(0, function.resultCount,  function.name);
			return getReturnValues<T...>(function.resultCount);
		}
		/*
			 0 input 0 
		*/
		void call(const LuaFunction<void>& function) const
		{
			loadFunction(function.name);
			callFunction(0, function.resultCount,  function.name);
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
				_open = true;
				_file = name;
			}
		}
		void closeFile() noexcept
		{
			lua_close(_state);
			_open = false;
		}
		bool changeFile(const std::string& name) noexcept
		{
			bool result = true;
			lua_close(_state);
			_state = luaL_newstate();
			try
			{
				loadFile(name);
				_file = name;
			}
			catch (const std::exception& )
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
		/*
			Loads  a lua file + standard lib and runs it
		*/
		void loadFile(const std::string& name) const
		{
			if (luaL_dofile(_state, name.c_str()))
			{
				generateError(lua_tostring(_state, -1));
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
		int loadFunctionParams(T&& value, Args&&... args) const
		{
			pushValue(value);
			return 1 + loadFunctionParams(args...);
		}
		template <typename T>
		int loadFunctionParams(T&& value) const
		{
			pushValue(value);
			return 1;
		}
		void callFunction(int inputValCount, int outputValCount, const std::string& functionName) const
		{
			if (lua_pcall(_state, inputValCount, outputValCount, 0) != 0) {
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
			if (sizeof...(T) > count)
			{
				throw std::invalid_argument("Number of template parameters passed is bigger than the number of the function's return values ");
			}
			else if (sizeof...(T) < count)
			{
				throw std::invalid_argument("The number of function return values is bigger than the number of template parameters for the return values");
			}
		}
		template <typename T>
		T getValue(int stackIndex = -1) const
		{
			if (!lua_istable(_state, -1))
			{
				std::string startText = "The variable you are trying to get is not a table, thus cannot be converted to a variable of type ";
				/*
				What we want here is just the name of the type thus we remove any references and cv qualifiers
				*/
				std::string endText = typeid(typename std::decay<T>::type).name();
				generateError(startText + endText);
			}
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
			size_t strLength = 0;
			const char* str = lua_tolstring(_state, stackIndex, &strLength);
			return std::string(str, strLength);
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
		std::tuple<Args ...> getReturnValues(unsigned int functionReturnCount) const
		{
			const unsigned int popCount = functionReturnCount;
			//The order of evaluation of an initializer list in c-tor of tuple<Args...> is well defined
			std::tuple<Args ...> result{ getReturnValue<Args>(functionReturnCount--) ... };
			popStack(popCount);
			return result;
		}
		template <typename T>
		T getReturnValue(int index) const
		{
			if (index != 0)
			{
				return  getValue<T>(index*-1);
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
			for (const auto& element : val.luaUnpack())
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
		std::string loadTable(std::string name) const
		{
			std::string parent = name.substr(0, name.find_first_of('.'));
			loadGlobalVariable(parent);
			return name.substr(name.find_first_of('.') + 1);
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
		void getTableField(const std::string& name, int index) const
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
		bool setField(const std::string& name, const T& val) const
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
