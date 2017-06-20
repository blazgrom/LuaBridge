#ifndef CAN_REPRESENT_VALUE_HPP
#define CAN_REPRESENT_VALUE_HPP
#include <limits>
#include <type_traits>
namespace Utils
{
	//Signed - Signed
	template <class OldT, class NewT>
	constexpr  typename std::enable_if<std::is_signed<OldT>::value && std::is_signed<NewT>::value, bool>::type can_represent_min()
	{
		return std::numeric_limits<OldT>::min() >= std::numeric_limits<NewT>::min();
	}
	//Unsigned - Unsigned
	template <class OldT, class NewT >
	constexpr typename std::enable_if<std::is_unsigned<OldT>::value && std::is_unsigned<NewT>::value, bool>::type can_represent_min()
	{
		return true;
	}
	//Unsigned - Signed
	template <class OldT, class NewT >
	constexpr typename std::enable_if<std::is_unsigned<OldT>::value && std::is_signed<NewT>::value, bool>::type can_represent_min()
	{
		return false;
	}
	//Signed - Unsigned
	template <class OldT, class NewT >
	constexpr typename std::enable_if<std::is_signed<OldT>::value && std::is_unsigned<NewT>::value, bool>::type can_represent_min()
	{
		return false;
	}
	template <class OldT, class NewT>
	constexpr bool can_represent_max()
	{
		//Note:
		//This will raise a warning signaling signed/unsigned mismatch, but the code works follows an explanation
		//The warning is raised only with the following combination of types
		//unsigned int - int , unsigned long - unsigned long , unsigned long long - long long
		//Explanation: When the calls to max() return we will have two values, one signed and one unsigned.
		//Based on the C++ rules of comparision between unsigned and signed, the signed value is promoted to unsigned,
		//and specificaly the unsigned we are comparing againts because the rules of C++ says that when type promotation
		//is needed, the value's type  is promoted to the first type that can represent it's correctly it's content, in our
		//case this is the unsigned  we are comparing against
		return (std::numeric_limits<OldT>::max() <= std::numeric_limits<NewT>::max());
	}
	template<class OldT, class NewT>
	constexpr bool  can_represent_value()
	{
		return can_represent_max<OldT, NewT>() && can_represent_min<OldT, NewT>();
	}
	//Struct for when you need type dispatching
	template <class OldT, class NewT>
	struct Can_represent_value :std::integral_constant<bool, can_represent_value<OldT, NewT>()> {};
}	
#endif // !CAN_REPRESENT_VALUE_HPP
