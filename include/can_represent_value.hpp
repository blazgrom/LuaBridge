#ifndef CAN_REPRESENT_VALUE_HPP
#define CAN_REPRESENT_VALUE_HPP
#include <limits>
#include <type_traits>
namespace Utils
{
	//Signed - Signed
	template <class OriginalT, class NewT>
	constexpr  typename std::enable_if<std::is_signed<OriginalT>::value && std::is_signed<NewT>::value, bool>::type can_represent_min()
	{
		return std::numeric_limits<OriginalT>::min() >= std::numeric_limits<NewT>::min();
	}
	//Unsigned - Unsigned
	template <class OriginalT, class NewT >
	constexpr typename std::enable_if<std::is_unsigned<OriginalT>::value && std::is_unsigned<NewT>::value, bool>::type can_represent_min()
	{
		return true;
	}
	//Unsigned - Signed
	template <class OriginalT, class NewT >
	constexpr typename std::enable_if<std::is_unsigned<OriginalT>::value && std::is_signed<NewT>::value, bool>::type can_represent_min()
	{
		return true;
	}	
	//Signed - Unsigned
	template <class OriginalT, class NewT >
	constexpr typename std::enable_if<std::is_signed<OriginalT>::value && std::is_unsigned<NewT>::value, bool>::type can_represent_min()
	{
		return false;
	}
	template <class OriginalT, class NewT>
	constexpr bool can_represent_max()
	{
		return (std::numeric_limits<OriginalT>::max() <= std::numeric_limits<NewT>::max());//signed - unsigned mismatch // TODO
	}
	template<class OriginalT, class NewT>
	constexpr bool  can_represent_value()
	{
		return can_represent_max<OriginalT, NewT>() && can_represent_min<OriginalT, NewT>();//signed - unsigned mismatch  // TODO
	}
	//Struct for when you need type dispatching
	template <class OriginalT, class NewT>
	struct Can_represent_value :std::integral_constant<bool, can_represent_value<OriginalT, NewT>()> {};
}	
#endif // !CAN_REPRESENT_VALUE_HPP
