#ifndef _ML_SINGLETON_HPP_
#define _ML_SINGLETON_HPP_

#include <modus_core/detail/NonCopyable.hpp>

namespace ml
{
	template <class Derived
	> struct singleton : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = typename Derived;

		ML_NODISCARD static self_type & get_singleton() noexcept
		{
			static self_type self{};
			return self;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SINGLETON_HPP_