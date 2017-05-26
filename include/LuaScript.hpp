#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <tuple>
#include <vector>
#include <string>
#include <utility>
#include <cassert>
#include <functional>
#include <unordered_map>
#include "lua.hpp"
#include "LuaTable.hpp"
#include "LuaFunction.hpp"
#include "callable_traits.hpp"
#include "can_represent_value.hpp"
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
		static std::string m_userF; // The name of the registered user function that will be called
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
		void register_function(const std::string& name, T& user_f);
	private:
		
		void init(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void runFile(const std::string& name) const;
		void retrieveLuaValue(const std::string& name) const;
		void retrieveGlobal(const std::string& name) const;
		void retrieveTableElement(const std::string& name) const;
		template <class T>
		bool setGlobal(const std::string& name, const T& val) const;
		template <class T>
		bool setTableElement(const std::string& name, const T& val) const;
		void prepareForCall(const std::string& name) const;
		void loadFunction(const std::string& name) const;
		void setUserFunctionToRun(const std::string& func) const;
		void callImpl(int inputCount, int outputCount, const std::string& name) const;
		void error(const std::string& message, bool popStack = false) const;
		void loadTableField(const std::string& field, int tableIndex = -1) const;
		void iterateTable(std::function<void(const std::string&)> predicate, bool popLastValue = true) const;
		LuaTable createLuaTable() const;
		template <class... Args>
		std::tuple<Args ...> getOutput(unsigned int count) const;
		template <class T>
		T getOutputImpl(int index) const;
		template <class T, class... Args>
		int setParameters(T&& value, Args&&... args) const;
		template <class T>
		int setParameters(T&& value) const;
		void registerFunctionImpl(const std::string& name);
		template <class T>
		struct RegisteredFunctionReturnType {};
		template <class T,class R, class... Args>
		int callRegisteredFunction(T& user_f, RegisteredFunctionReturnType<R>& , std::tuple<Args...>&);
		template <class T, class... Args>
		int callRegisteredFunction(T& user_f, RegisteredFunctionReturnType<void>& , std::tuple<Args...>&);
		void popLuaStack(int count = 1) const;
		template <class T>
		T topLuaStack()const;
		template <class T>
		T getLuaStack(int index = -1) const;
		template<class T>
		T getLuaNumber(int index, std::true_type) const;
		template <class T>
		T getLuaNumber(int index, std::false_type) const;
		template <class T>
		T getLuaInteger(int index, std::true_type) const;
		template <class T>
		T getLuaInteger(int index, std::false_type) const;
		template <class T>
		typename std::enable_if<std::is_convertible<T, Lua::LuaTable>::value>::type pushLuaStack(T val) const;
		void pushLuaStack(std::nullptr_t) const;
		void pushLuaStack(std::string val) const;
		void pushLuaStack(char val) const;
		void pushLuaStack(bool val) const;
		template<typename = typename std::enable_if<Utils::can_represent_value<short, lua_Integer>()>::type>
		void pushLuaStack(short val) const;
		template<typename = typename std::enable_if<Utils::can_represent_value<unsigned short, lua_Integer>()>::type>
		void pushLuaStack(unsigned short val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<int, lua_Integer>()>::type>
		void pushLuaStack(int val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned int, lua_Integer>()>::type>
		void pushLuaStack(unsigned int val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<long, lua_Integer>()>::type>
		void pushLuaStack(long val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long, lua_Integer>()>::type>
		void pushLuaStack(unsigned long val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<long long, lua_Integer>()>::type>
		void pushLuaStack(long long val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long long, lua_Integer>()>::type>
		void pushLuaStack(unsigned long long val) const;
		template <typename = typename std::enable_if < Utils::can_represent_value <float, lua_Number>()>::type >
		void pushLuaStack(float val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<double, lua_Number>()>::type>
		void pushLuaStack(double val) const;
	};
	template <class T>
	T LuaScript::get(const std::string& name) const
	{
		retrieveLuaValue(name);
		return topLuaStack<T>();
	}
	template <class T>
	bool LuaScript::set(const std::string& name, T&& val) const
	{
		if (name.find('.') == std::string::npos)
			return setGlobal(name, val);
		else
			return setTableElement(name, val);
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
	bool LuaScript::setTableElement(const std::string& name, const T& val) const
	{
		try
		{
			std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
			retrieveGlobal(tableName);
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
	template <class... R, class... Args>
	std::tuple<R...> LuaScript::call(const LuaFunction<R...>& f, Args&&... args) const
	{
		prepareForCall(f.name);
		const int inputCount = setParameters(std::forward<Args>(args)...);
		callImpl(inputCount, f.resultCount, f.name);
		return getOutput<R ...>(f.resultCount);
	}
	template <class First, class Second, class... Args>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f, Args&&... args) const
	{
		prepareForCall(f.name);
		const int inputCount = setParameters(std::forward<Args>(args)...);
		const int outputCount = 2;
		callImpl(inputCount, outputCount, f.name);
		auto temp_tuple = getOutput<First, Second>(f.resultCount);
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T, class... Args>
	T LuaScript::call(const LuaFunction<T>& f, Args&&... args) const
	{
		prepareForCall(f.name);
		const int inputCount = setParameters(std::forward<Args>(args)...);
		callImpl(inputCount, 1, f.name);
		return std::get<0>(getOutput<T>(f.resultCount));
	}
	template <class...Args>
	void LuaScript::call(const LuaFunction<void>& f, Args&&... args) const
	{
		prepareForCall(f.name);
		const int inputCount = setParameters(std::forward<Args>(args)...);
		callImpl(inputCount, f.resultCount, f.name);
	}
	template <class... T>
	std::tuple<T...> LuaScript::call(const LuaFunction<T...>& f) const
	{
		prepareForCall(f.name);
		const int inputCount = 0;
		callImpl(inputCount, f.resultCount, f.name);
		return getOutput<T...>(f.resultCount);
	}
	template <class First, class Second>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f) const
	{
		prepareForCall(f.name);
		const int inputCount = 0, outputCount = 2;
		callImpl(inputCount, outputCount, f.name);
		auto temp_tuple=getOutput<T>(f.resultCount);
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T>
	T LuaScript::call(const LuaFunction<T>& f) const
	{
		prepareForCall(f.name);
		const int outputCount = 1,inputCount=0;
		callImpl(inputCount, outputCount, f.name);
		return std::get<0>(getOutput<T>(f.resultCount));
	}
	template <class T, class... Args>
	int LuaScript::setParameters(T&& value, Args&&... args) const
	{
		pushLuaStack(value);
		return 1 + setParameters(args...);
	}
	template <class T>
	int LuaScript::setParameters(T&& value) const
	{
		pushLuaStack(value);
		return 1;
	}
	template <class... Args>
	std::tuple<Args ...> LuaScript::getOutput(unsigned int count) const
	{
		const int outputCount = count;
		std::tuple<Args ...> result={ getOutputImpl<Args>(count--) ... };
		popLuaStack(outputCount);
		return result;
	}
	template <class T>
	T LuaScript::getOutputImpl(int index) const
	{
		if (index != 0)
			return  getLuaStack<T>(index*-1);
		else
			throw std::runtime_error("You cannot get return values, because there are none");
	}
	template <class T>
	void LuaScript::F()
	{

	}
	template<class R, class... Args>
	void LuaScript::register_function(const std::string& name, std::function<R(Args...)>& user_f)
	{
		m_localFunctions.push_back(name);
		LuaScript::m_userFunctions[name] = [this, user_f](lua_State*)->int {
			int inputCount = lua_gettop(m_state);
			if (inputCount==sizeof...(Args))
			{
				std::tuple<Args...> arguments={};
				RegisteredFunctionReturnType<R> returnType = {};
				return callRegisteredFunction(user_f, returnType, arguments);
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
	void LuaScript::register_function(const std::string& name, T& user_f)
	{
		m_localFunctions.push_back(name);
		LuaScript::m_userFunctions[name] = [this, user_f](lua_State*) mutable ->int   {
			int stackTop = lua_gettop(m_state);
			if (stackTop == Utils::callable_traits<T>::args_count)
			{
				Utils::callable_arg_types<T> arguments = {};
				RegisteredFunctionReturnType<Utils::callable_return_type<T>> returnType = {};
				return callRegisteredFunction(user_f, returnType, arguments);
			}
			return 0;
		};
		registerFunctionImpl(name);
	}
	//Utilities
	template <class T,class R, class... Args >
	int LuaScript::callRegisteredFunction(T& user_f, RegisteredFunctionReturnType<R>& ,std::tuple<Args...>&)
	{
		auto result = user_f(topLuaStack<Args>()...);//This won't work when passing data from lua to c++, test it !
		pushLuaStack(result);
		return 1;
	}
	template <class T, class... Args>
	int LuaScript::callRegisteredFunction(T& user_f, RegisteredFunctionReturnType<void>& , std::tuple<Args...>&)
	{
		user_f(topLuaStack<Args>()...);//This won't work when passing data from lua to c++, test it !
		return 0;
	}

	template <class T>
	T LuaScript::topLuaStack()const
	{
		const int topElement = -1;
		T result = getLuaStack<T>(topElement);
		popLuaStack();
		return result;
	}
	template <class T>
	T LuaScript::getLuaStack(int) const
	{
		static_assert(std::is_constructible<T, LuaTable>::value, "Type cannot be constructed from a LuaTable");
		if (!lua_istable(m_state, -1))
		{
			error("The type you are trying to retrieve cannot be constructed with a LuaTable");
		}
		return T{ createLuaTable() };
	}
	template<>
	inline std::string LuaScript::getLuaStack<std::string>(int index) const
	{
		size_t strLength = 0;
		const char* str = lua_tolstring(m_state, index, &strLength);
		std::string r{ str, strLength };
		return r;
	}
	template <>
	inline char LuaScript::getLuaStack<char>(int index) const
	{
		auto str = getLuaStack<std::string>(index);
		if (str.length() > 1)
		{
			throw std::runtime_error("The value you are trying to retrieve has more than one characters");
		}
		return str[0];
	}
	template <>
	inline bool LuaScript::getLuaStack<bool>(int index) const
	{
		return lua_toboolean(m_state, index) != 0;
	}
	template <>
	inline short LuaScript::getLuaStack<short>(int index) const
	{
		return getLuaInteger<short>(index, Utils::Can_represent_value<lua_Integer, short>{});
	}
	template <>
	inline unsigned short LuaScript::getLuaStack<unsigned short>(int index) const
	{
		return getLuaInteger<unsigned short>(index, Utils::Can_represent_value<lua_Integer,unsigned short>{});
	}
	template <>
	inline int LuaScript::getLuaStack<int>(int index) const
	{
		return getLuaInteger<int>(index, Utils::Can_represent_value<lua_Integer, int>{});
	}
	template <>
	inline unsigned int LuaScript::getLuaStack<unsigned int>(int index) const
	{
		return getLuaInteger<unsigned int>(index, Utils::Can_represent_value<lua_Integer,unsigned int>{});
	}
	template <>
	inline long LuaScript::getLuaStack<long>(int index) const
	{
		return getLuaInteger<long>(index, Utils::Can_represent_value<lua_Integer, long>{});
	}
	template <>
	inline unsigned long LuaScript::getLuaStack<unsigned long>(int index) const
	{
		return getLuaInteger<unsigned long>(index, Utils::Can_represent_value<lua_Integer,unsigned long>{});
	}
	template <>
	inline long long LuaScript::getLuaStack<long long>(int index) const
	{
		return getLuaInteger<long long>(index, Utils::Can_represent_value<lua_Integer,long long>{});
	}
	template <>
	inline unsigned long long LuaScript::getLuaStack<unsigned long long>(int index) const
	{
		return getLuaInteger<unsigned long long>(index, Utils::Can_represent_value<lua_Integer, unsigned long long>{});
	}
	template <>
	inline double LuaScript::getLuaStack<double>(int index) const
	{
		return getLuaNumber<double>(index, Utils::Can_represent_value<lua_Number, double>{});
	}
	template <>
	inline float LuaScript::getLuaStack<float>(int index) const
	{
		return getLuaNumber<float>(index, Utils::Can_represent_value<lua_Number, float>{});
	}
	template<class T>
	T LuaScript::getLuaNumber(int index, std::true_type) const
	{
		T result=static_cast<T>(lua_tonumber(m_state, index));
		return result;
	}
	template <class T>
	T LuaScript::getLuaNumber(int index, std::false_type) const
	{
		assert(false, "The type " + typeid(T).name() + "cannot represent a lua_Number");
		return T{};
	}
	template <class T>
	T LuaScript::getLuaInteger(int index, std::true_type) const
	{
		T result = static_cast<T>(lua_tointeger(m_state, index));
		return result;
	}
	template <class T>
	T LuaScript::getLuaInteger(int index, std::false_type) const
	{
		assert(false, "The type " + typeid(T).name() + "cannot represent a lua_Integer");
		return T{};
	}
	template <class T>
	typename std::enable_if<std::is_convertible<T, Lua::LuaTable>::value>::type LuaScript::pushLuaStack(T val) const
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
	template<typename = typename std::enable_if<Utils::can_represent_value<short, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(short val) const
	{
		lua_pushinteger(m_state, val);
	}
	template<typename = typename std::enable_if<Utils::can_represent_value<unsigned short, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(unsigned short val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename = typename std::enable_if<Utils::can_represent_value<int, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(int val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename = typename std::enable_if<Utils::can_represent_value<unsigned int, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(unsigned int val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename = typename std::enable_if<Utils::can_represent_value<long, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(unsigned long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename = typename std::enable_if<Utils::can_represent_value<long long, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(long long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long long, lua_Integer>()>::type>
	void LuaScript::pushLuaStack(unsigned long long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename = typename std::enable_if < Utils::can_represent_value <float, lua_Number>()>::type >
	void LuaScript::pushLuaStack(float val) const
	{
		lua_pushnumber(m_state, val);
	}
	template <typename = typename std::enable_if<Utils::can_represent_value<double, lua_Number>()>::type>
	void LuaScript::pushLuaStack(double val) const
	{
		lua_pushnumber(m_state, val);
	}
}
#endif // !LUA_SCRIPT_HPP

