#ifndef VARIADIC_INDEX_HPP
#define VARIADIC_INDEX_HPP
namespace Utils
{
	template<class...Ts>
	struct variadric_index
	{
		int get_index()
		{
			return  index++;
		}
		int get_reverse_index()
		{
			return reverse_index--;
		}
	private:
		size_t reverse_index = sizeof...(Ts)-1;
		size_t index = 0;
	};
}
#endif // !VARIADIC_INDEX_HPP
