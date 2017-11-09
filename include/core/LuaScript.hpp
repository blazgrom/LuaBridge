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
#include "LuaStack.hpp"
#include "LuaFunction.hpp"
#include "variadic_index.hpp"
#include "callable_traits.hpp"
#include "can_represent_value.hpp"
#include "various.hpp"
namespace LuaBz
{
	class LuaScript
	{
	private:
		using LuaCF_Intermediary = std::function<int(lua_State*)>;
		LuaStack m_stack;
		std::string m_fileName;
		bool m_open;
		static std::vector<LuaCF_Intermediary> m_registeredFunctions;
	public:
		LuaScript();
		explicit LuaScript(const std::string& file, bool loadStandardLib = false);
		LuaScript(const std::string& file, const std::vector<std::string>& dependencies, bool loadStandardLib =false);
		LuaScript(const std::string& file, const std::string& dependency, bool loadStandardLib = false);
		~LuaScript()=default;
		LuaScript(const LuaScript& rhs) = delete;
		LuaScript& operator=(const LuaScript& rhs) = delete;
		LuaScript(LuaScript&& rhs) = default;
		LuaScript& operator=(LuaScript&& rhs) = default;
		void open(const std::string& file,std::vector<std::string> dependencies={},bool loadStandardLib=false);
		void close() noexcept;
		bool change(const std::string& newFile) noexcept;
		template <class T>
		T get(const std::string& name) const;
		template <class T>
		void set(const std::string& name, T&& val) const;
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
		template<class R, class... Args>
		void register_function(const std::string& name, std::function<R(Args...)>& user_f);
		template<class R, class... Args>
		void register_function(const std::string& name, R(*user_f)(Args...));
		template<class T>
		void register_function(const std::string& name, T& user_f);
	private:
		void load_function(const std::string& name) const;
		template <class... Args>
		std::tuple<Args ...> get_output(unsigned int count) const;
		template <class T>
		T get_output_impl(int index) const;
		template <class T, class... Args>
		int set_parameters(T&& value, Args&&... args) const;
		template <class T>
		int set_parameters(T&& value) const;
		void register_function_impl(const std::string& name);
		template <class T,class R, class... Args>
		int call_registered_function(T& user_f, Utils::type_container<R> , std::tuple<Args...>&);
		template <class T, class... Args>
		int call_registered_function(T& user_f, Utils::type_container<void> , std::tuple<Args...>&);
	};
	template <class T>
	T LuaScript::get(const std::string& name) const
	{
		m_stack.set_top_element(name);
		return m_stack.top_element<T>(true);
	}
	template <class T>
	void LuaScript::set(const std::string& name, T&& val) const
	{
		m_stack.set_element(name, val);
	}
	template <class... R, class... Args>
	std::tuple<R...> LuaScript::call(const LuaFunction<R...>& f, Args&&... args) const
	{
		load_function(f.name());
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		m_stack.call_function(inputCount, f.result_count());
		return get_output<R ...>(f.result_count());
	}
	template <class First, class Second, class... Args>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f, Args&&... args) const
	{
		load_function(f.name());
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		const int outputCount = 2;
		m_stack.call_function(inputCount, outputCount);
		auto temp_tuple = get_output<First, Second>(f.result_count());
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T, class... Args>
	T LuaScript::call(const LuaFunction<T>& f, Args&&... args) const
	{
		load_function(f.name());
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		m_stack.call_function(inputCount, 1);
		return std::get<0>(get_output<T>(f.result_count()));
	}
	template <class...Args>
	void LuaScript::call(const LuaFunction<void>& f, Args&&... args) const
	{
		load_function(f.name());
		const int inputCount = set_parameters(std::forward<Args>(args)...);
		m_stack.call_function(inputCount, f.result_count());
	}
	template <class... T>
	std::tuple<T...> LuaScript::call(const LuaFunction<T...>& f) const
	{
		load_function(f.name());
		const int inputCount = 0;
		m_stack.call_function(inputCount, f.result_count());
		return get_output<T...>(f.result_count());
	}
	template <class First, class Second>
	std::pair<First, Second> LuaScript::call(const LuaFunction<First, Second>& f) const
	{
		load_function(f.name());
		const int inputCount = 0, outputCount = 2;
		m_stack.call_function(inputCount, outputCount);
		auto temp_tuple=get_output<First,Second>(f.result_count());
		return std::make_pair(std::get<0>(temp_tuple), std::get<1>(temp_tuple));
	}
	template <class T>
	T LuaScript::call(const LuaFunction<T>& f) const
	{
		load_function(f.name());
		const int outputCount = 1,inputCount=0;
		m_stack.call_function(inputCount, outputCount);
		return std::get<0>(get_output<T>(f.result_count()));
	}
	template <class T, class... Args>
	int LuaScript::set_parameters(T&& value, Args&&... args) const
	{
		m_stack.push(value);
		return 1 + set_parameters(args...);
	}
	template <class T>
	int LuaScript::set_parameters(T&& value) const
	{
		m_stack.push(value);
		return 1;
	}
	template <class... Args>
	std::tuple<Args ...> LuaScript::get_output(unsigned int count) const
	{
		const int outputCount = count;
		std::tuple<Args ...> result={ get_output_impl<Args>(count--) ... };
		m_stack.pop(outputCount);
		return result;
	}
	template <class T>
	T LuaScript::get_output_impl(int index) const
	{
		return  m_stack.get_element<T>(index*-1);
	}
	template<class R, class... Args>
	void LuaScript::register_function(const std::string& name, std::function<R(Args...)>& user_f)
	{
		//WARNING: Not thread safe
		m_registeredFunctions.emplace_back([this, user_f](lua_State*)->int {
			if (m_stack.size() == sizeof...(Args))
			{
				std::tuple<Args...> arguments = {};
				Utils::type_container<R> returnType = {};
				return call_registered_function(user_f, returnType, arguments);
			}
			return 0;
		});
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
			if (m_stack.size() == Utils::callable_traits<T>::args_count)
			{
				Utils::callable_arg_types<T> arguments = {};
				Utils::type_container<Utils::callable_return_type<T>> returnType = {};
				return call_registered_function(user_f, returnType, arguments);
			}
			return 0;
		});
		register_function_impl(name);
	}
	//Utilities
	template <class T,class R, class... Args >
	int LuaScript::call_registered_function(T& user_f, Utils::type_container<R> ,std::tuple<Args...>& )
	{
		//TODO: Find solution for the following problem
		//auto result = user_f(m_stack.get_element<Args>((index_generator.get_index() + 1)*-1)...);
		//This solution works under Microsoft  compiler and GCC compiler
		Utils::variadric_index<Args...> index_generator;
		auto result = user_f(m_stack.get_element<Args>((index_generator.get_index() + 1)*-1)...);
		m_stack.pop(sizeof...(Args));
		m_stack.push(result);
		return 1;
	}
	template <class T, class... Args>
	int LuaScript::call_registered_function(T& user_f, Utils::type_container<void>, std::tuple<Args...>& )
	{
		
		//TODO: Find solution for the following problem
		//user_f(m_stack.get_element<Args>((index_generator.get_index() + 1)*-1)...);
		//This solution works under Microsoft  compiler and GCC compiler
		Utils::variadric_index<Args...> index_generator;
		user_f(m_stack.get_element<Args>((index_generator.get_index() + 1)*-1)...);
		m_stack.pop(sizeof...(Args));
		return 0;
	}
}
#endif // !LUABZ_LUA_SCRIPT_HPP