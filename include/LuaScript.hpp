#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <tuple>
#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <unordered_map>
#include "lua.hpp"
#include "LuaTable.hpp"
#include "LuaFunction.hpp"
#include "tuple_utils.hpp"
#include "callable_traits.hpp"
namespace Lua
{

	class LuaScript
	{
	private:
		lua_State* m_state;
		std::string m_fileName;
		bool m_open;
		std::vector<std::string> m_localFunctions;
		static std::unordered_map<std::string, std::function<int(lua_State*)>> m_userFunctions;
		static std::string m_userFunctionInCall;
	public:
		explicit LuaScript(const std::string& file, bool loadStandardLib = true);
		LuaScript(const std::string& file, const std::vector<std::string>& dependencies, bool loadStandardLib = true);
		LuaScript(const std::string& file, const std::string& dependency, bool loadStandardLib = true);
		~LuaScript();
		LuaScript(const LuaScript& rhs) = delete;
		LuaScript& operator=(const LuaScript& rhs) = delete;
		LuaScript(LuaScript&& rhs) = default;
		LuaScript& operator=(LuaScript&& rhs) = default;
		void open(const std::string& file);
		void close() noexcept;
		bool change(const std::string& newFile) noexcept;
		template <class T>
		T get(const std::string& name) const;
		template <class T>
		bool set(const std::string& name, T&& val) const;



		template <class... R, class... Args>
		std::tuple<R...> call(const LuaFunction<R...>& f, Args&&... args) const;
		template <class First,class Second, class... Args>
		std::pair<First,Second> call(const LuaFunction<First,Second>& f, Args&&... args) const;
		template <class T, class... Args>
		T call(const LuaFunction<T>& f, Args&&... args) const;
		template <class...Args>
		void call(const LuaFunction<void>& f, Args&&... args) const;
		template <class... T>
		std::tuple<T...> call(const LuaFunction<T...>& f) const;
		template <class T>
		T call(const LuaFunction<T>& f) const;
		template <class First,class Second>
		std::pair<First,Second> call(const LuaFunction<First,Second>& f) const;
		void call(const LuaFunction<void>& f) const;
		void run(std::string luaCode);
		std::unordered_map<std::string, std::string> tableInfo(const std::string& table) const;
		template<class R, class... Args>
		void register_function(const std::string& name, std::function<R(Args...)>& user_f);
		template<class R, class... Args>
		void register_function(const std::string& name, R(*user_f)(Args...));
		template<class T>
		void register_function(const std::string& name, T user_f);
	private:
		void initialize(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void loadGlobal(const std::string& name) const;
		void loadTabElement(const std::string& name) const;
		template <class T>
		bool setGlobal(const std::string& name, const T& val) const;
		template <class T>
		bool setTabElement(const std::string& name, const T& val) const;
		void loadFile(const std::string& name) const;
		void loadFunction(const std::string& name) const;
		void call_Impl(int inputCount, int outputCount, const std::string& name) const;
		void error(const std::string& message, bool popStack = false) const;
		void loadTableField(const std::string& field, int tableIndex = -1) const;
		void iterateTable(std::function<void(const std::string&)> predicate, bool popLastValue = true) const;
		LuaTable createLuaTable() const;
		template <class... Args>
		std::tuple<Args ...> getFunctionResult(unsigned int count) const;
		template <class T>
		T getFunctionResult_Impl(int index) const;
		template <class T, class... Args>
		int setFunctionParamaters(T&& value, Args&&... args) const;
		template <class T>
		int setFunctionParamaters(T&& value) const;
		void registerFunctionImpl(const std::string& name);
		template <class T,class... Args>
		int callMemberFunction(T& user_f, std::tuple<Args...>&);
		template <class T>
		T topLuaStack(int index = -1) const;
		template <class T>
		void pushLuaStack(const T& val) const;
		void popLuaStack(int count = 1) const;
	};
	//Get/set
	template <class T>
	T LuaScript::get(const std::string& name) const
	{
		if (name.find('.') == std::string::npos)
			loadGlobal(name);
		else
			loadTabElement(name);
		return topLuaStack<T>();
	}
	template <class T>
	bool LuaScript::set(const std::string& name, T&& val) const
	{
		if (name.find('.') == std::string::npos)
			return setGlobal(name, val);
		else
			return setTabElement(name, val);
	}
	template <class T>
	bool LuaScript::setGlobal(const std::string& name, const T& val) const
	{
		try
		{
			pushLuaStack(val);
			lua_setglobal(m_state, name.c_str());
			return true;
		}
		catch (const std::runtime_error&)
		{
			return false;
		}
	}
	template <class T>
	bool LuaScript::setTabElement(const std::string& name, const T& val) const
	{
		try
		{
			std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
			loadGlobal(tableName);
			auto keepProcessing = true;
			while (keepProcessing)
			{
				auto dotPosition = tableField.find_first_of('.');
				if (dotPosition == std::string::npos)
				{
					pushLuaStack<T>(val);
					lua_setfield(m_state, -2, tableField.c_str());
					keepProcessing = false;
				}
				else
				{
					std::string parent = tableField.substr(0, dotPosition);
					tableField = tableField.substr(dotPosition + 1);
					loadTableField(parent);
				}
			}
			return true;
		}
		catch (const std::runtime_error&)
		{
			return false;
		}
	}
	//Function call 
	template <class... R, class... Args>
	std::tuple<R...> LuaScript::call(const LuaFunction<R...>& f, Args&&... args) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
		call_Impl(argumentsCount, f.resultCount, f.name);
		return getFunctionResult<R ...>(f.resultCount);
	}
	template <class First, class Second, class... Args>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f, Args&&... args) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
		call_Impl(argumentsCount, 2, f.name);
		auto temp_tuple = getFunctionResult<First, Second>(f.resultCount);
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T, class... Args>
	T LuaScript::call(const LuaFunction<T>& f, Args&&... args) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
		call_Impl(argumentsCount, 1, f.name);
		return std::get<0>(getFunctionResult<T>(f.resultCount));
	}
	template <class...Args>
	void LuaScript::call(const LuaFunction<void>& f, Args&&... args) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
		call_Impl(argumentsCount, f.resultCount, f.name);
	}
	template <class... T>
	std::tuple<T...> LuaScript::call(const LuaFunction<T...>& f) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		call_Impl(0, f.resultCount, f.name);
		return getFunctionResult<T...>(f.resultCount);
	}
	template <class First, class Second>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		call_Impl(0, 2, f.name);
		auto temp_tuple=getFunctionResult<T>(f.resultCount);
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T>
	T LuaScript::call(const LuaFunction<T>& f) const
	{
		if (LuaScript::m_userFunctions.find(f.name) != LuaScript::m_userFunctions.end())
		{
			LuaScript::m_userFunctionInCall = f.name;
		}
		loadFunction(f.name);
		call_Impl(0, 1, f.name);
		return std::get<0>(getFunctionResult<T>(f.resultCount));
	}
	//Function utilities
	template <class T, class... Args>
	int LuaScript::setFunctionParamaters(T&& value, Args&&... args) const
	{
		pushLuaStack(value);
		return 1 + setFunctionParamaters(args...);
	}
	template <class T>
	int LuaScript::setFunctionParamaters(T&& value) const
	{
		pushLuaStack(value);
		return 1;
	}
	template <class... Args>
	std::tuple<Args ...> LuaScript::getFunctionResult(unsigned int count) const
	{
		return{ getFunctionResult_Impl<Args>(count--) ... };
	}
	template <class T>
	T LuaScript::getFunctionResult_Impl(int index) const
	{
		if (index != 0)
			return  topLuaStack<T>(index*-1);
		else
			throw std::runtime_error("You cannot get return values, because there are none");
	}
	//Register C++ function
	template<class R, class... Args>
	void LuaScript::register_function(const std::string& name, std::function<R(Args...)>& user_f)
	{
		m_localFunctions.push_back(name);
		LuaScript::m_userFunctions[name] = [this, user_f](lua_State*)->int {
			int stackTop = lua_gettop(m_state);
			if (stackTop != 0)
			{
				auto result = user_f(topLuaStack<Args>()...);
				pushLuaStack(result);
				return 1;
			}
			return 0;
		};
		registerFunctionImpl(name);
	}
	template<class R, class... Args>
	void LuaScript::register_function(const std::string& name, R(*user_f)(Args...))
	{
		std::function<R(Args...)> func = user_f;
		register_function(name, func);
	}
	template<class T>
	void LuaScript::register_function(const std::string& name, T user_f)
	{
		m_localFunctions.push_back(name);
		LuaScript::m_userFunctions[name] = [this, user_f](lua_State*) mutable ->int   {
			int stackTop = lua_gettop(m_state);
			if (stackTop != 0)
			{
				Utils::callable_arg_types<T> t;
				return callMemberFunction(user_f, t);
			}
			return 0;
		};
		registerFunctionImpl(name);
	}
	//Utilities
	template <class T, class... Args>
	int LuaScript::callMemberFunction(T& user_f, std::tuple<Args...>&)
	{
		auto result = user_f(topLuaStack<Args>()...);
		pushLuaStack(result);
		return 1;
	}
	template <class T>
	T LuaScript::topLuaStack(int) const
	{
		if (!lua_istable(m_state, -1))
		{
			error("The type you are trying to retrieve cannot be constructed with a LuaTable");
		}
		return T{ createLuaTable() };
	}
	template <>
	inline double LuaScript::topLuaStack<double>(int index) const
	{
		auto r = static_cast<double>(lua_tonumber(m_state, index));
		popLuaStack();
		return r;
	}
	template <>
	inline float LuaScript::topLuaStack<float>(int index) const
	{
		auto r = static_cast<float>(topLuaStack<double>(index));
		popLuaStack();
		return r;
	}
	template <>
	inline int LuaScript::topLuaStack<int>(int index) const
	{
		auto r = static_cast<int>(lua_tointeger(m_state, index));
		popLuaStack();
		return r;
	}
	template<>
	inline std::string LuaScript::topLuaStack<std::string>(int index) const
	{
		size_t strLength = 0;
		const char* str = lua_tolstring(m_state, index, &strLength);
		std::string r{ str, strLength };
		popLuaStack();
		return r;
	}
	template <>
	inline bool LuaScript::topLuaStack<bool>(int index) const
	{
		auto  r = lua_toboolean(m_state, index) != 0;
		popLuaStack();
		return r;
	}
	template <class T>
	void LuaScript::pushLuaStack(const T& val) const
	{
		lua_newtable(m_state);
		auto table = static_cast<LuaTable>(val);
		for (const auto& element : table.values)
		{
			pushLuaStack(element.name());
			switch (element.type())
			{
			case LuaType::Boolean:
				pushLuaStack(element.boolean());
				break;
			case LuaType::Number:
				pushLuaStack(element.number());
				break;
			case LuaType::Integer:
				pushLuaStack(element.integer());
				break;
			case LuaType::Nil:
				pushLuaStack(element.nil());
				break;
			case LuaType::String:
				pushLuaStack(element.string());
				break;
			}
			lua_settable(m_state, -3); //automatically pops [key,value] 
		}
	}
	template <>
	inline void LuaScript::pushLuaStack<std::nullptr_t>(const std::nullptr_t&) const
	{
		lua_pushnil(m_state);
	}
	template <>
	inline void LuaScript::pushLuaStack<bool>(const bool& val) const
	{
		lua_pushboolean(m_state, val);
	}
	template<>
	inline void LuaScript::pushLuaStack<std::string>(const std::string& val) const
	{
		lua_pushlstring(m_state, val.c_str(), val.size());
	}
	template <>
	inline void LuaScript::pushLuaStack<int>(const int& val) const
	{
		lua_pushinteger(m_state, val);
	}
	template<>
	inline void LuaScript::pushLuaStack<double>(const double& val) const
	{
		lua_pushnumber(m_state, val);
	}
	template<>
	inline void LuaScript::pushLuaStack<float>(const float& val) const
	{
		pushLuaStack(static_cast<double>(val));
	}
}
#endif // !LUA_SCRIPT_HPP

