#ifndef _ML_SINGLETON_HPP_
#define _ML_SINGLETON_HPP_

#include <detail/NonCopyable.hpp>

namespace ml
{
	template <class Derived
	> struct singleton : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = typename Derived;

		ML_NODISCARD static self_type * get_singleton() noexcept
		{
			static self_type self{};
			return std::addressof(self);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SINGLETON_HPP_