#ifndef _ML_LIST_HPP_
#define _ML_LIST_HPP_

// WIP / PLACEHOLDER

#include <modus_core/detail/Utility.hpp>

namespace ml
{
	// list
	template <
		class Ty,
		class Al = pmr::polymorphic_allocator<Ty>
	> ML_alias list = typename std::vector
	<
		Ty, Al
	>;
}

namespace ml::util
{
	template <class T, class V = T
	> auto & find_or_add(list<T> & l, V && value) noexcept
	{
		if (auto const it{ std::find(l.begin(), l.end(), ML_forward(value)) }
		; it != l.end())
		{
			return *it;
		}
		else
		{
			return l.emplace_back(ML_forward(value));
		}
	}

	template <class T, class Pr = std::less<T>, class V = T
	> auto find_or_insert(list<T> & l, V && value) noexcept
	{
		if (auto const it{ std::equal_range(l.begin(), l.end(), ML_forward(value), Pr{}) }
		; it.first != it.second)
		{
			return it.second;
		}
		else
		{
			return l.emplace(it.second, ML_forward(value));
		}
	}
}

#endif // !_ML_LIST_HPP_