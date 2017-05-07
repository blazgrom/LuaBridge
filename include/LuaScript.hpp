#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP
#include <map>
#include <tuple>
#include <vector>
#include <iostream>
#include <string>
#include <functional>
#include <type_traits>
#include "LuaHelpers.hpp"
#include "lua.hpp"
namespace Lua
{

	class LuaScript
	{
	private:
		lua_State* m_state;
		std::string m_fileName;
		bool m_open;
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


		//template <class T, class... Args>
		//void register_function(std::string name,std::function<T(Args...)>& user_input)
		//{
		//	lua_CFunction a = [&user_input]()->lua_CFunction {
		//		static auto static_user_input = user_input;
		//		struct FunctionCallHelper
		//		{
		//			static T call(Args&& args)
		//			{
		//				return static_user_input(std::forward<Args>(args)...);
		//			}
		//			static T call()
		//			{
		//				return static_user_input();
		//			}
		//		};
		//		return [](lua_State*)->int {
		//			//Retrieve sizeof...(Args) variables from the stack and pass them to call
		//			std::cout << FunctionCallHelper::call() << "\n";
		//			return 1;
		//		};
		//	}();
		//	a(m_state);
		//}
	
		template <class T>
		void register_function(std::string name, T user_function)
		{
			auto temp{ user_function };
			std::cout<<typeid(typename function_traits<decltype(temp)>::result_type).name()<<"\n";
			auto argument_cout = function_traits<decltype(temp)>::argument_count;
		}
		void funct() 
		{
			std::function<std::string()> user_input = []()->std::string {
				return "aaaa\n";
			};
			std::function<std::string()> user_input1 = []()->std::string {
				return "aaaa11111\n";
			};
			lua_CFunction a=[user_input]()->lua_CFunction {
				struct internal
				{
					static std::string call(std::function<std::string()> m_user_input)
					{
						return m_user_input();
					}
				};
				static auto user = user_input;
				return [](lua_State*)->int {
					std::cout<<internal::call(user)<<"\n";
					return 2;
				};
			}();
			a(nullptr);
			a= [user_input1]()->lua_CFunction {
				struct internal
				{
					static std::string call(std::function<std::string()> m_user_input)
					{
						return m_user_input();
					}
				};
				static auto user = user_input1;
				return [](lua_State*)->int {
					std::cout << internal::call(user) << "\n";
					return 2;
				};
			}();
			a(nullptr);
		}
	private:
		void initialize(const std::vector<std::string>& dependencies, bool loadStandardLib);
		void loadFile(const std::string& name) const;
		void loadFunction(const std::string& name) const;
		void call_Impl(int inputCount, int outputCount, const std::string& name) const;
		void pop(int count = 1) const;
		void error(const std::string& message, bool popStack = false) const;
		void loadTableField(const std::string& field, int tableIndex = -1) const;
		void traverseTable(std::function<void(const std::string&)> predicate, bool popLastValue = true) const;
		LuaTable createLuaTable() const;
		template <class T>
		T get_Impl(int stackIndex = -1) const;
		template <class T>
		void push(const T& val) const;
		void push(const std::nullptr_t) const;
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
		bool setGlobalVariable(const std::string& name, T&& val) const;
		template <class T>
		bool setTableField(const std::string& name,T&& val) const;
		void getGlobalVariable(const std::string& name) const;
		void getTableField(const std::string& name) const;
	};
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
			return setGlobalVariable(name, std::forward<T>(val));
		else
			return setTableField(name, std::forward<T>(val));
	}
	template <class... R, class... Args>
	std::tuple<R...> LuaScript::call(const LuaFunction<R...>& f, Args&&... args) const
	{
		loadFunction(f.name);
		int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
		call_Impl(argumentsCount, f.resultCount, f.name);
		return getFunctionResult<R ...>(f.resultCount);
	}
	template <class...Args>
	void LuaScript::call(const LuaFunction<void>& f, Args&&... args) const
	{
		loadFunction(f.name);
		int argumentsCount = setFunctionParamaters(std::forward<Args>(args)...);
		call_Impl(argumentsCount, f.resultCount, f.name);
	}
	template <class... T>
	std::tuple<T...> LuaScript::call(const LuaFunction<T...>& f) const
	{
		loadFunction(f.name);
		call_Impl(0, f.resultCount, f.name);
		return getFunctionResult<T...>(f.resultCount);
	}
	template <class T>
	T LuaScript::get_Impl(int) const
	{
		if (!lua_istable(m_state, -1))
		{
			error("The Lua variable you are trying to retrieve is not a table, thus cannot be converted to a user defined type");
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
	template <class T>
	void LuaScript::push(const T& val) const
	{
		lua_newtable(m_state);
		auto table = static_cast<LuaTable>(val);
		for (const auto& element : table.values)
		{
			push(element.name());//key
			switch (element.type())//value
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
			lua_settable(m_state, -3); //automatically pops the key and the value from the stack
		}
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
	template <class T>
	bool LuaScript::setGlobalVariable(const std::string& name, T&& val) const
	{
		try
		{
			push(std::forward<T>(val));
			lua_setglobal(m_state, name.c_str());
			return true;
		}
		catch (const std::runtime_error&)
		{
			return false;
		}
	}
	template <class T>
	bool LuaScript::setTableField(const std::string& name, T&& val) const
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
					push(std::forward<T>(val));
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
}
#endif // !LUA_SCRIPT_HPP

