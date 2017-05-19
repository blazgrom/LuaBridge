#ifndef TUPLE_UTILS_HPP
#define TUPLE_UTILS_HPP
#include <tuple>
namespace Utils
{
	template <size_t T>
	struct tuple_index {};
	template <size_t Index, class... Args>
	void for_each_tuple(std::tuple<Args...> t, tuple_index<Index>)
	{
		using tuple_element_type = typename std::tuple_element<Index, decltype(t)>::type;
		for_each_tuple(t, tuple_index<Index - 1>{});
	}
	template<class... Args>
	void for_each_tuple(std::tuple<Args...> t, tuple_index<0>)
	{
		using tuple_element_type = typename std::tuple_element<0, decltype(t)>::type;
	}
	inline void for_each_tuple(std::tuple<> t, tuple_index<0>)
	{
	} 
	template<class... Args>
	constexpr size_t Tuple_size(std::tuple<Args...>&)
	{
		return (std::tuple_size<std::tuple<Args...>>::value != 0) ? std::tuple_size<std::tuple<Args...>>::value - 1 : std::tuple_size<std::tuple<Args...>>::value;
	}
}
#endif // !TUPLE_UTILS_HPP
