#ifndef _ML_GLOBALS_HPP_
#define _ML_GLOBALS_HPP_

#include <modus_core/Common.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace globals
	{
		// get global implementation
		template <class Type
		> ML_NODISCARD Type * get() noexcept
		{
			static_assert(0, "global getter not defined for type");
			return nullptr;
		}

		// set global implementation
		template <class Type
		> Type * set(Type *) noexcept
		{
			static_assert(0, "global setter not defined for type");
			return nullptr;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get global
	template <class Type
	> ML_NODISCARD Type * get_global() noexcept
	{
		return _ML globals::get<Type>();
	}

	// set global
	template <class Type
	> Type * set_global(Type * value) noexcept
	{
		return _ML globals::set<Type>(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_GLOBALS_HPP_