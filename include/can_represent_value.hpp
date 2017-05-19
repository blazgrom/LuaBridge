#ifndef CAN_REPRESENT_VALUE_HPP
#define CAN_REPRESENT_VALUE_HPP
#include <limits>
#include <type_traits>
namespace Utils
{
	template <class OriginalT, class NewT>
	constexpr  typename std::enable_if<std::is_signed<OriginalT>::value && std::is_signed<NewT>::value, bool>::type can_represent_min()
	{
		return std::numeric_limits<OriginalT>::min() >= std::numeric_limits<NewT>::min();
	}
	template <class OriginalT, class NewT >
	constexpr typename std::enable_if<std::is_unsigned<OriginalT>::value && std::is_unsigned<NewT>::value, bool>::type can_represent_min()
	{
		return true;
	}
	template <class OriginalT, class NewT>
	constexpr bool can_represent_max()
	{
		return (std::numeric_limits<OriginalT>::max() <= std::numeric_limits<NewT>::max());
	}
	template<class OriginalT, class NewT>
	constexpr bool  can_represent_value()
	{
		return can_represent_max<OriginalT, NewT>() && can_represent_min<OriginalT, NewT>();
	}
}	
#endif // !CAN_REPRESENT_VALUE_HPP
