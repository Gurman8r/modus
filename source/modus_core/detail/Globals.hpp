#ifndef _ML_GLOBALS_HPP_
#define _ML_GLOBALS_HPP_

#include <modus_core/Common.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace globals
	{
		// get global implementation
		template <class Type> ML_NODISCARD Type * get() noexcept
		{
			static_assert(0, "global get not implemented");
			return nullptr;
		}

		// set global implementation
		template <class Type> Type * set(Type *) noexcept
		{
			static_assert(0, "global set not implemented");
			return nullptr;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get global
	template <class T> ML_NODISCARD auto get_global() noexcept
	{
		return _ML globals::get<std::_Remove_cvref_t<T>>();
	}

	// set global
	template <class T> auto set_global(T * value) noexcept
	{
		return _ML globals::set<std::_Remove_cvref_t<T>>(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_GLOBALS_HPP_