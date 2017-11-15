#ifndef LUABZ_LUA_STACK_HPP
#define LUABZ_LUA_STACK_HPP
#include <vector>
#include <algorithm>
#include <cassert>
#include <string>
#include <functional>
#include <tuple>
#include "lua.hpp"
#include "lua_table.hpp"
#include "lua_error.hpp"
#include "can_represent_value.hpp"
#include "static_type_if.hpp"
#include "lua_type.hpp"
namespace LuaBz
{
	class lua_stack
	{
	public:
		lua_stack();
		lua_stack(bool loadStandardLib, const std::string& file, std::vector<std::string> dependencies = {});
		~lua_stack();
		void pop(int count = 1) const;
		void set_top_element(const std::string& name) const;
		template <class T>
		T top_element(bool popTopElement = false) const;
		template <class T>
		void set_element(const std::string& name, T&& value) const;
		template <class T>
		T get_element(int index) const;
		//TODO?:Why this uses std::enable_if !?
		template <class T>
		typename std::enable_if<std::is_convertible<T, lua_table>::value>::type push(T val) const;
		void push(const lua_table& val) const;
		void push(lua_CFunction function) const;
		void push(std::nullptr_t) const;
		void push(std::string val) const;
		void push(char val) const;
		void push(bool val) const;
		void push(short val) const;
		void push(int val) const;
		void push(unsigned int val) const;
		void push(long val) const;
		void push(unsigned long val) const;
		void push(long long val) const;
		void push(unsigned long long val) const;
		void push(float val) const;
		void push(double val) const;
		void call_function(int inputCount, int outputCount) const;
		bool is_function(int index = -1) const;
		int size() const;
		void execute(const std::string& code) const;
		void destroy();
		void create(const std::string& file,const std::vector<std::string>& dependencies={},bool loadStandardLib=false);
	private:
		lua_State* m_state;
		static const int topElement = -1;
		template <class T>
		void set_global_variable(const std::string& name, T&& value) const;
		template <class T>
		void set_table_element(const std::string& name, const T& val) const;
		void run_file(const std::string& name) const;
		void get_global_variable(const std::string& name) const;
		void get_table_element(const std::string& name) const;
		void load_table_field(const std::string& name) const;
		template<class T>
		T get_lua_number(int index, std::true_type) const;
		template <class T>
		T get_lua_number(int , std::false_type) const;
		template <class T>
		T get_lua_integer(int index, std::true_type) const;
		template <class T>
		T get_lua_integer(int , std::false_type) const;
		template <class T>
		T get(int index) const;
		template <typename T>
		T get_impl(int index,detail::ClassType) const;
		template<typename T>
		T get_impl(int index,detail::IntegralType) const;
		template<typename T>
		T get_impl(int index,detail::FloatingPointType) const;
		lua_table create_lua_table() const;
		void iterate_table(std::function<void(const std::string&)> callback) const;
	};
	#ifndef LUA_STACK_TEMPLATE_IMPL
	#define LUA_STACK_TEMPLATE_IMPL
		#include "lua_stack.tpp"
	#undef LUA_STACK_TEMPLATE_IMPL
	#endif

}
#endif // !LUABZ_LUA_STACK_HPP