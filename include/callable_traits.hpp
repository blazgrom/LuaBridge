#ifndef CALLABLE_TRAITS_HPP
#define CALLABLE_TRAITS_HPP
#include <functional>
namespace Utils
{
	template <class T>
	struct callable_traits_impl // Intermediary for classes and lambdas
	{
	private:
		template<class R, class C = T, class... Args>
		static constexpr auto getOperator(R(C::*op)(Args...))
		{
			return  op;
		}

	public:
		using return_type = typename callable_traits_impl<decltype(getOperator(&T::operator()))>::return_type;
		using args = typename callable_traits_impl<decltype(getOperator(&T::operator()))>::args;
		static const size_t args_count = callable_traits_impl<decltype(getOperator(&T::operator()))>::args_count;
	};
	template <class T>
	struct callable_traits_impl<const T> // Intermediary for const classes and lambdas
	{
	private:
		template<class R, class C = T, class... Args>
		static constexpr auto getConstOperator(R(C::*op)(Args...) const)
		{
			return  op;
		}
	public:
		using return_type = typename callable_traits_impl<decltype(getConstOperator(&T::operator()))>::return_type;
		using args = typename callable_traits_impl<decltype(getConstOperator(&T::operator()))>::args;
		static const size_t args_count = callable_traits_impl<decltype(getConstOperator(&T::operator()))>::args_count;
	};
	template <class R, class... Args>
	struct callable_traits_impl < std::function<R(Args...)>>
	{
		using return_type = R;
		using args = std::tuple<Args...>;
		static const size_t args_count = sizeof...(Args);
	};
	template<class R, class... Args>
	struct callable_traits_impl < R(*)(Args...)>
	{
		using return_type = R;
		using args = std::tuple<Args...>;
		static const size_t args_count = sizeof...(Args);
	};
	template<class R, class C, class... Args>
	struct callable_traits_impl <R(C::*)(Args...) const volatile>
	{
		using return_type = R;
		using args = std::tuple<Args...>;
		static const size_t args_count = sizeof...(Args);
	};
	template<class R, class C, class... Args>
	struct callable_traits_impl <R(C::*)(Args...) const>
	{
		using return_type = R;
		using args = std::tuple<Args...>;
		static const size_t args_count = sizeof...(Args);
	};
	template<class R, class C, class... Args>
	struct callable_traits_impl <R(C::*)(Args...) volatile>
	{
		using return_type = R;
		using args = std::tuple<Args...>;
		static const size_t args_count = sizeof...(Args);
	};
	template<class R, class C, class... Args>
	struct callable_traits_impl <R(C::*)(Args...)>
	{
		using return_type = R;
		using args = std::tuple<Args...>;
		static const size_t args_count = sizeof...(Args);
	};
	template <class T>
	struct callable_traits
	{
		using return_type = typename callable_traits_impl<T>::return_type;
		using args = typename callable_traits_impl<T>::args;
		static const size_t args_count = callable_traits_impl<T>::args_count;
	};
	template<class T>
	using callable_return_type = typename callable_traits<T>::return_type;
	template<class T>
	using callable_arg_types = typename callable_traits<T>::args;
}
#endif // !CALLABLE_TRAITS_HPP