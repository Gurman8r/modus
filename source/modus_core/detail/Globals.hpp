#ifndef _ML_GLOBALS_HPP_
#define _ML_GLOBALS_HPP_

#include <modus_core/detail/Debug.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// globals api
#ifndef ML_GLOBALS_API
#define ML_GLOBALS_API ML_CORE_API
#endif

 // globals namespace
#define _ML_GLOBALS _ML globals::

// globals declaration ( header.hpp )
#define ML_decl_global(T) template <> ML_NODISCARD ML_GLOBALS_API T *

// globals implementation ( source.cpp )
#define ML_impl_global(T) template <> T *

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::globals
{
	// get global
	template <class T> ML_NODISCARD T * get() noexcept
	{
		static_assert(0, "get global not implemented for type");
		return nullptr;
	}

	// set global
	template <class T> ML_NODISCARD T * set(T *) noexcept
	{
		static_assert(0, "set global not implemented for type");
		return nullptr;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// get global macro
#define ML_get_global(T)			(_ML_GLOBALS get<T>())

// set global macro
#define ML_set_global(T, value)		(_ML_GLOBALS set<T>(value))

// begin global macro
#define ML_begin_global(T, value)	((value) && (ML_get_global(T) == nullptr) && ML_set_global(T, (value)))

// end global macro
#define ML_end_global(T, value)		((value) && (ML_get_global(T) == (value)) && !ML_set_global(T, nullptr))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_GLOBALS_HPP_