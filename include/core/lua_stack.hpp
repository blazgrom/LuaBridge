#ifndef LUABZ_LUA_STACK_HPP
#define LUABZ_LUA_STACK_HPP
#include <vector>
#include <algorithm>
#include <cassert>
#include <string>
#include <functional>
#include "lua.hpp"
#include "lua_table.hpp"
#include "lua_error.hpp"
#include "can_represent_value.hpp"
namespace LuaBz
{
	//TODO: Remove all implicetly inlined functions
	class lua_stack
	{
	public:
		lua_stack()
			:
			m_state{nullptr}
		{

		}
		lua_stack(bool loadStandardLib, const std::string& file, std::vector<std::string> dependencies = {})
			:
			m_state{ nullptr }
		{
			create(file,dependencies,loadStandardLib);
		}
		~lua_stack()
		{
			lua_close(m_state);
		}
		void pop(int count = 1) const;
		void set_top_element(const std::string& name) const;
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
		void set_element(const std::string& name, T&& value) const
		{
			if (name.find('.') == std::string::npos)
				set_global_variable(name, value);
			else
				set_table_element(name, value);
		}
		template <class T>
		T get_element(int index) const
		{
			auto d = get<T>(index);
			return d;
		}
		template <class T>
		typename std::enable_if<std::is_convertible<T, lua_table>::value>::type push(T val) const
		{
			auto table = static_cast<lua_table>(val);
			push(table);
		}
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
		void set_global_variable(const std::string& name, T&& value) const
		{
				push(value);
				lua_setglobal(m_state, name.c_str());
		}
		template <class T>
		void set_table_element(const std::string& name, const T& val) const
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
		}
		void run_file(const std::string& name) const;
		void get_global_variable(const std::string& name) const;
		void get_table_element(const std::string& name) const;
		void load_table_field(const std::string& name) const;
		template<class T>
		T get_lua_number(int index, std::true_type) const
		{
			T result = static_cast<T>(lua_tonumber(m_state, index));
			return result;
		}
		template <class T>
		T get_lua_number(int , std::false_type) const
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
		T get_lua_integer(int , std::false_type) const
		{
			assert(false);	
			return T{};
		}
		template <class T>
		T get(int index) const
		{
			static_assert(std::is_constructible<T, lua_table>::value, "Type cannot be constructed from a lua_table");
			return T{ get<lua_table>(index)};
		}
	
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