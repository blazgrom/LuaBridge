#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <map>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <string>
#include <functional>
#include "LuaHelpers.hpp"
#include "callable_traits.hpp"
#include "tuple_utils.hpp"
#include "lua.hpp"
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
		template <class...Args>
		void call(const LuaFunction<void>& f, Args&&... args) const;
		template <class... T>
		std::tuple<T...> call(const LuaFunction<T...>& f) const;
		void call(const LuaFunction<void>& f) const;
		void run(std::string luaCode);
		std::map<std::string, std::string> tableInfo(const std::string& table) const;
		template<class R, class... Args>
		void register_function(const std::string& name, std::function<R(Args...)>& user_f);
		template<class R, class... Args>
		void register_function(const std::string& name, R(*user_f)(Args...));
		template<class T>
		void register_function(const std::string& name, T user_f);
	private:
		void initialize(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void loadFile(const std::string& name) const;
		void loadFunction(const std::string& name) const;
		void call_Impl(int inputCount, int outputCount, const std::string& name) const;
		void pop(int count = 1) const;
		void error(const std::string& message, bool popStack = false) const;
		void loadTableField(const std::string& field, int tableIndex = -1) const;
		void iterateTable(std::function<void(const std::string&)> predicate, bool popLastValue = true) const;
		LuaTable createLuaTable() const;
		template <class T>
		T get_Impl(int stackIndex = -1) const;
		template <class T>
		void push(const T& val) const;
		void push(std::nullptr_t&) const;
		void push(bool val) const;
		void push(const std::string& val) const;
		void push(int val) const;
		void push(double val) const;
		void push(float val) const;
		template <class... Args>
		std::tuple<Args ...> getFunctionResult(unsigned int count) const;
		template <class T>
		T getFunctionResult_Impl(int index) const;
		template <class T, class... Args>
		int setFunctionParamaters(T&& value, Args&&... args) const;
		template <class T>
		int setFunctionParamaters(T&& value) const;
		template <class T>
		bool setGlobalVariable(const std::string& name, const T& val) const;
		template <class T>
		bool setTableField(const std::string& name, const T& val) const;
		void getGlobalVariable(const std::string& name) const;
		void getTableField(const std::string& name) const;
		void registerFunctionImpl(const std::string& name);
		template <class T,class... Args>
		int callMemberFunction(T& user_f, std::tuple<Args...>&);
	};
	//Get/set
	template <class T>
	T LuaScript::get(const std::string& name) const
	{
		if (name.find('.') == std::string::npos)
			getGlobalVariable(name);
		else
			getTableField(name);
		return get_Impl<T>();
	}
	template <class T>
	bool LuaScript::set(const std::string& name, T&& val) const
	{
		if (name.find('.') == std::string::npos)
			return setGlobalVariable(name, val);
		else
			return setTableField(name, val);
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
	//Get implementations
	template <class T>
	T LuaScript::get_Impl(int) const
	{
		if (!lua_istable(m_state, -1))
		{
			error("The type you are trying to retrieve cannot be constructed with a LuaTable");
		}
		return T{ createLuaTable() };
	}
	template <>
	inline double LuaScript::get_Impl<double>(int stackIndex) const
	{
		auto r = static_cast<double>(lua_tonumber(m_state, stackIndex));
		pop();
		return r;
	}
	template <>
	inline float LuaScript::get_Impl<float>(int stackIndex) const
	{
		auto r = static_cast<float>(get_Impl<double>(stackIndex));
		pop();
		return r;
	}
	template <>
	inline int LuaScript::get_Impl<int>(int stackIndex) const
	{
		auto r = static_cast<int>(lua_tointeger(m_state, stackIndex));
		pop();
		return r;
	}
	template<>
	inline std::string LuaScript::get_Impl<std::string>(int stackIndex) const
	{
		size_t strLength = 0;
		const char* str = lua_tolstring(m_state, stackIndex, &strLength);
		std::string r{ str, strLength };
		pop();
		return r;
	}
	template <>
	inline bool LuaScript::get_Impl<bool>(int stackIndex) const
	{
		auto  r = lua_toboolean(m_state, stackIndex) != 0;
		pop();
		return r;
	}
	//Generic push
	template <class T>
	void LuaScript::push(const T& val) const
	{
		lua_newtable(m_state);
		auto table = static_cast<LuaTable>(val);
		for (const auto& element : table.values)
		{
			push(element.name());
			switch (element.type())
			{
			case LuaType::Boolean:
				push(element.boolean());
				break;
			case LuaType::Number:
				push(element.number());
				break;
			case LuaType::Integer:
				push(element.integer());
				break;
			case LuaType::Nil:
				push(element.nil());
				break;
			case LuaType::String:
				push(element.string());
				break;
			}
			lua_settable(m_state, -3); //automatically pops [key,value] 
		}
	}
	//Function utilities
	template <class T, class... Args>
	int LuaScript::setFunctionParamaters(T&& value, Args&&... args) const
	{
		push(value);
		return 1 + setFunctionParamaters(args...);
	}
	template <class T>
	int LuaScript::setFunctionParamaters(T&& value) const
	{
		push(value);
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
			return  get_Impl<T>(index*-1);
		else
			throw std::runtime_error("You cannot get return values, because there are none");
	}
	//Set implementations
	template <class T>
	bool LuaScript::setGlobalVariable(const std::string& name, const T& val) const
	{
		try
		{
			push(val);
			lua_setglobal(m_state, name.c_str());
			return true;
		}
		catch (const std::runtime_error&)
		{
			return false;
		}
	}
	template <class T>
	bool LuaScript::setTableField(const std::string& name, const T& val) const
	{
		try
		{
			std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
			getGlobalVariable(tableName);
			auto keepProcessing = true;
			while (keepProcessing)
			{
				auto dotPosition = tableField.find_first_of('.');
				if (dotPosition == std::string::npos)
				{
					push<T>(val);
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
	//Register C++ function
	template<class R, class... Args>
	void LuaScript::register_function(const std::string& name, std::function<R(Args...)>& user_f)
	{
		m_localFunctions.push_back(name);
		LuaScript::m_userFunctions[name] = [this, user_f](lua_State*)->int {
			int stackTop = lua_gettop(m_state);
			if (stackTop != 0)
			{
				auto result = user_f(get_Impl<Args>()...);
				push(result);
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
		LuaScript::m_userFunctions[name] = [this, user_f](lua_State*)->int {
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
	//Register C++ function utilities
	template <class T, class... Args>
	int LuaScript::callMemberFunction(T& user_f, std::tuple<Args...>&)
	{
		auto result = user_f(get_Impl<Args>()...);
		push(result);
		return 1;
	}
}
#endif // !LUA_SCRIPT_HPP

