#ifndef LUABZ_LUA_SCRIPT_HPP
#define LUABZ_LUA_SCRIPT_HPP
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
namespace LuaBz
{
	class LuaScript
	{
	private:
		using LuaCF_Intermediary = std::function<int(lua_State*)>;
		lua_State* m_state;
		std::string m_fileName;
		bool m_open;
		std::vector<int> m_localFunctions; //TODO: Every instance of LuaScript should be able to call onyl function registered to itself
		static std::vector<LuaCF_Intermediary> m_registeredFunctions;
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
		std::unordered_map<std::string, std::string> table_info(const std::string& table) const;
		template<class R, class... Args>
		void register_function(const std::string& name, std::function<R(Args...)>& user_f);
		template<class R, class... Args>
		void register_function(const std::string& name, R(*user_f)(Args...));
		template<class T>
		void register_function(const std::string& name, T& user_f);
	private:
		void init(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void run_file(const std::string& name) const;
		void retrieve_lua_value(const std::string& name) const;
		void retrieve_global(const std::string& name) const;
		void retrieve_table_element(const std::string& name) const;
		template <class T>
		bool set_global(const std::string& name, const T& val) const;
		template <class T>
		bool set_table_element(const std::string& name, const T& val) const;
		void prepare_for_call(const std::string& name) const;
		void load_function(const std::string& name) const;
		void call_impl(int inputCount, int outputCount, const std::string& name) const;
		void error(const std::string& message, bool popStack = false) const;
		void load_table_field(const std::string& field, int tableIndex = -1) const;
		void iterate_table(std::function<void(const std::string&)> predicate, bool popLastValue = true) const;
		LuaTable create_lua_table() const;
		template <class... Args>
		std::tuple<Args ...> get_output(unsigned int count) const;
		template <class T>
		T get_output_impl(int index) const;
		template <class T, class... Args>
		int set_parameters(T&& value, Args&&... args) const;
		template <class T>
		int set_parameters(T&& value) const;
		void register_function_impl(const std::string& name);
		template <class T>
		struct RegisteredFunctionReturnType {};
		template <class T,class R, class... Args>
		int call_registered_function(T& user_f, RegisteredFunctionReturnType<R>& , std::tuple<Args...>&);
		template <class T, class... Args>
		int call_registered_function(T& user_f, RegisteredFunctionReturnType<void>& , std::tuple<Args...>&);
		void pop_lua_stack(int count = 1) const;
		template <class T>
		T get_pop_lua_stack()const;
		template <class T>
		T get_lua_stack(int index = -1) const;
		template<class T>
		T get_lua_number(int index, std::true_type) const;
		template <class T>
		T get_lua_number(int index, std::false_type) const;
		template <class T>
		T get_lua_integer(int index, std::true_type) const;
		template <class T>
		T get_lua_integer(int index, std::false_type) const;
		template <class T>
		typename std::enable_if<std::is_convertible<T, LuaTable>::value>::type push_lua_stack(T val) const;
		void push_lua_stack(const LuaTable& val) const;
		void push_lua_stack(std::nullptr_t) const;
		void push_lua_stack(std::string val) const;
		void push_lua_stack(char val) const;
		void push_lua_stack(bool val) const;
		template<typename = typename std::enable_if<Utils::can_represent_value<short, lua_Integer>()>::type>
		void push_lua_stack(short val) const;
		template<typename = typename std::enable_if<Utils::can_represent_value<unsigned short, lua_Integer>()>::type>
		void push_lua_stack(unsigned short val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<int, lua_Integer>()>::type>
		void push_lua_stack(int val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned int, lua_Integer>()>::type>
		void push_lua_stack(unsigned int val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<long, lua_Integer>()>::type>
		void push_lua_stack(long val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long, lua_Integer>()>::type>
		void push_lua_stack(unsigned long val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<long long, lua_Integer>()>::type>
		void push_lua_stack(long long val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<unsigned long long, lua_Integer>()>::type>
		void push_lua_stack(unsigned long long val) const;
		template <typename = typename std::enable_if < Utils::can_represent_value <float, lua_Number>()>::type >
		void push_lua_stack(float val) const;
		template <typename = typename std::enable_if<Utils::can_represent_value<double, lua_Number>()>::type>
		void push_lua_stack(double val) const;
		
	};
	template <class T>
	T LuaScript::get(const std::string& name) const
	{
		retrieve_lua_value(name);
		return get_pop_lua_stack<T>();
	}
	template <class T>
	bool LuaScript::set(const std::string& name, T&& val) const
	{
		if (name.find('.') == std::string::npos)
			return set_global(name, val);
		else
			return set_table_element(name, val);
	}
	template <class T>
	bool LuaScript::set_global(const std::string& name, const T& val) const
	{
		try
		{
			push_lua_stack(val);
			lua_setglobal(m_state, name.c_str());
			return true;
		}
		catch (const std::runtime_error&)
		{
			return false;
		}
	}
	template <class T>
	bool LuaScript::set_table_element(const std::string& name, const T& val) const
	{
		try
		{
			std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
			retrieve_global(tableName);
			auto keepProcessing = true;
			while (keepProcessing)
			{
				auto dotPosition = tableField.find_first_of('.');
				if (dotPosition == std::string::npos)
				{
					push_lua_stack<T>(val);
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
	template <class... R, class... Args>
	std::tuple<R...> LuaScript::call(const LuaFunction<R...>& f, Args&&... args) const
	{
		prepare_for_call(f.name);
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		call_impl(inputCount, f.resultCount, f.name);
		return get_output<R ...>(f.resultCount);
	}
	template <class First, class Second, class... Args>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f, Args&&... args) const
	{
		prepare_for_call(f.name);
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		const int outputCount = 2;
		call_impl(inputCount, outputCount, f.name);
		auto temp_tuple = get_output<First, Second>(f.resultCount);
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T, class... Args>
	T LuaScript::call(const LuaFunction<T>& f, Args&&... args) const
	{
		prepare_for_call(f.name);
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		call_impl(inputCount, 1, f.name);
		return std::get<0>(get_output<T>(f.resultCount));
	}
	template <class...Args>
	void LuaScript::call(const LuaFunction<void>& f, Args&&... args) const
	{
		prepare_for_call(f.name);
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		call_impl(inputCount, f.resultCount, f.name);
	}
	template <class... T>
	std::tuple<T...> LuaScript::call(const LuaFunction<T...>& f) const
	{
		prepare_for_call(f.name);
		const int inputCount = 0;
		call_impl(inputCount, f.resultCount, f.name);
		return get_output<T...>(f.resultCount);
	}
	template <class First, class Second>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f) const
	{
		prepare_for_call(f.name);
		const int inputCount = 0, outputCount = 2;
		call_impl(inputCount, outputCount, f.name);
		auto temp_tuple=get_output<T>(f.resultCount);
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T>
	T LuaScript::call(const LuaFunction<T>& f) const
	{
		prepare_for_call(f.name);
		const int outputCount = 1,inputCount=0;
		call_impl(inputCount, outputCount, f.name);
		return std::get<0>(get_output<T>(f.resultCount));
	}
	template <class T, class... Args>
	int LuaScript::set_parameters(T&& value, Args&&... args) const
	{
		push_lua_stack(value);
		return 1 + set_parameters(args...);
	}
	template <class T>
	int LuaScript::set_parameters(T&& value) const
	{
		push_lua_stack(value);
		return 1;
	}
	template <class... Args>
	std::tuple<Args ...> LuaScript::get_output(unsigned int count) const
	{
		const int outputCount = count;
		std::tuple<Args ...> result={ get_output_impl<Args>(count--) ... };
		pop_lua_stack(outputCount);
		return result;
	}
	template <class T>
	T LuaScript::get_output_impl(int index) const
	{
		if (index != 0)
			return  get_lua_stack<T>(index*-1);
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
		//WARNING: Not thread safe
		m_registeredFunctions.emplace_back([this, user_f](lua_State*)->int {
			int inputCount = lua_gettop(m_state);
			if (inputCount == sizeof...(Args))
			{
				std::tuple<Args...> arguments = {};
				RegisteredFunctionReturnType<R> returnType = {};
				return call_registered_function(user_f, returnType, arguments);
			}
			return 0;
		});
		//m_localFunctions.push_back(m_registeredFunctions.size()-1);

		register_function_impl(name);
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
		//WARNING: Not thread safe
		m_registeredFunctions.emplace_back([this, user_f](lua_State*) mutable ->int {
			int stackTop = lua_gettop(m_state);
			if (stackTop == Utils::callable_traits<T>::args_count)
			{
				Utils::callable_arg_types<T> arguments = {};
				RegisteredFunctionReturnType<Utils::callable_return_type<T>> returnType = {};
				return call_registered_function(user_f, returnType, arguments);
			}
			return 0;
		});
		//m_localFunctions.push_back(m_registeredFunctions.size() - 1);

		register_function_impl(name);
	}
	//Utilities
	template <class T,class R, class... Args >
	int LuaScript::call_registered_function(T& user_f, RegisteredFunctionReturnType<R>& ,std::tuple<Args...>&)
	{
		auto result = user_f(get_pop_lua_stack<Args>()...);//TODO:This won't work when passing data from lua to c++, test it !
		push_lua_stack(result);
		return 1;
	}
	template <class T, class... Args>
	int LuaScript::call_registered_function(T& user_f, RegisteredFunctionReturnType<void>& , std::tuple<Args...>&)
	{
		user_f(get_pop_lua_stack<Args>()...);//TODO:This won't work when passing data from lua to c++, test it !
		return 0;
	}

	template <class T>
	T LuaScript::get_pop_lua_stack() const//TODO :Change the name of this to something like getPop
	{
		const int topElement = -1;
		T result = get_lua_stack<T>(topElement);
		pop_lua_stack();
		return result;
	}
	template <class T>
	T LuaScript::get_lua_stack(int index) const
	{
		static_assert(std::is_constructible<T, LuaTable>::value, "Type cannot be constructed from a LuaTable");
		if (!lua_istable(m_state, index))
		{
			error("The type you are trying to retrieve cannot be constructed with a LuaTable");
		}
		return T{ create_lua_table() };
	}
	template<>
	inline LuaTable LuaScript::get_lua_stack(int index) const
	{
		if (!lua_istable(m_state, index))
		{
			error("The type you are trying to retrieve cannot be constructed with a LuaTable");
		}
		return  create_lua_table();
	}
	template<>
	inline std::string LuaScript::get_lua_stack<std::string>(int index) const
	{
		size_t strLength = 0;
		const char* str = lua_tolstring(m_state, index, &strLength);
		std::string r{ str, strLength };
		return r;
	}
	template <>
	inline char LuaScript::get_lua_stack<char>(int index) const
	{
		auto str = get_lua_stack<std::string>(index);
		if (str.length() > 1)
		{
			throw std::runtime_error("The value you are trying to retrieve has more than one characters");
		}
		return str[0];
	}
	template <>
	inline bool LuaScript::get_lua_stack<bool>(int index) const
	{
		return lua_toboolean(m_state, index) != 0;
	}
	template <>
	inline short LuaScript::get_lua_stack<short>(int index) const
	{
		return get_lua_integer<short>(index, Utils::Can_represent_value<lua_Integer, short>{});
	}
	template <>
	inline unsigned short LuaScript::get_lua_stack<unsigned short>(int index) const
	{
		return get_lua_integer<unsigned short>(index, Utils::Can_represent_value<lua_Integer,unsigned short>{});
	}
	template <>
	inline int LuaScript::get_lua_stack<int>(int index) const
	{
		return get_lua_integer<int>(index, Utils::Can_represent_value<lua_Integer, int>{});
	}
	template <>
	inline unsigned int LuaScript::get_lua_stack<unsigned int>(int index) const
	{
		return get_lua_integer<unsigned int>(index, Utils::Can_represent_value<lua_Integer,unsigned int>{});
	}
	template <>
	inline long LuaScript::get_lua_stack<long>(int index) const
	{
		return get_lua_integer<long>(index, Utils::Can_represent_value<lua_Integer, long>{});
	}
	template <>
	inline unsigned long LuaScript::get_lua_stack<unsigned long>(int index) const
	{
		return get_lua_integer<unsigned long>(index, Utils::Can_represent_value<lua_Integer,unsigned long>{});
	}
	template <>
	inline long long LuaScript::get_lua_stack<long long>(int index) const
	{
		return get_lua_integer<long long>(index, Utils::Can_represent_value<lua_Integer,long long>{});
	}
	template <>
	inline unsigned long long LuaScript::get_lua_stack<unsigned long long>(int index) const
	{
		return get_lua_integer<unsigned long long>(index, Utils::Can_represent_value<lua_Integer, unsigned long long>{});
	}
	template <>
	inline double LuaScript::get_lua_stack<double>(int index) const
	{
		return get_lua_number<double>(index, Utils::Can_represent_value<lua_Number, double>{});
	}
	template <>
	inline float LuaScript::get_lua_stack<float>(int index) const
	{
		return get_lua_number<float>(index, Utils::Can_represent_value<lua_Number, float>{});
	}
	template<class T>
	T LuaScript::get_lua_number(int index, std::true_type) const
	{
		T result=static_cast<T>(lua_tonumber(m_state, index));
		return result;
	}
	template <class T>
	T LuaScript::get_lua_number(int index, std::false_type) const
	{
		assert(false);
		return T{};
	}
	template <class T>
	T LuaScript::get_lua_integer(int index, std::true_type) const
	{
		T result = static_cast<T>(lua_tointeger(m_state, index));
		return result;
	}
	template <class T>
	T LuaScript::get_lua_integer(int index, std::false_type) const
	{
		assert(false);
		return T{};
	}
	template <class T>
	typename std::enable_if<std::is_convertible<T, LuaTable>::value>::type LuaScript::push_lua_stack(T val) const
	{
		auto table = static_cast<LuaTable>(val);
		push_lua_stack(table);
	}
	template<typename>
	void LuaScript::push_lua_stack(short val) const
	{
		lua_pushinteger(m_state, val);
	}
	template<typename>
	void LuaScript::push_lua_stack(unsigned short val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(int val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(unsigned int val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(unsigned long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(long long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(unsigned long long val) const
	{
		lua_pushinteger(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(float val) const
	{
		lua_pushnumber(m_state, val);
	}
	template <typename>
	void LuaScript::push_lua_stack(double val) const
	{
		lua_pushnumber(m_state, val);
	}
}
#endif // !LUABZ_LUA_SCRIPT_HPP