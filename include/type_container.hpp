#ifndef TYPE_CONTAINER_HPP
#define TYPE_CONTAINER_HPP
namespace Utils
{
	//Class used for when we need to pass types as data to a function
	template<class... Ts>
	struct type_container{};
}
#endif // !TYPE_CONTAINER_HPP
