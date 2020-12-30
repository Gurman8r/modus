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

// get global
#define ML_get_global(T)			(_ML_GLOBALS get_global<T>())

// set global
#define ML_set_global(T, value)		(_ML_GLOBALS set_global<T>(value))

// begin global
#define ML_begin_global(T, value)	(_ML_GLOBALS begin_global<T>(value))

// end global
#define ML_end_global(T, value)		(_ML_GLOBALS end_global<T>(value))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::globals
{
	// get global
	template <class T> ML_NODISCARD T * get_global() noexcept
	{
		static_assert(0, "get global not implemented for type");
		return nullptr;
	}

	// set global
	template <class T> ML_NODISCARD T * set_global(T *) noexcept
	{
		static_assert(0, "set global not implemented for type");
		return nullptr;
	}

	// begin global
	template <class T> ML_NODISCARD bool begin_global(T * value) noexcept
	{
		return value && (ML_get_global(T) == nullptr) && ML_set_global(T, value);
	}

	// end global
	template <class T> ML_NODISCARD bool end_global(T * value) noexcept
	{
		return value && (ML_get_global(T) == value) && !ML_set_global(T, nullptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_GLOBALS_HPP_