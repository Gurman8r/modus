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

// is global
#define ML_is_global(T, value)		(_ML_GLOBALS is_global<T>(value))

// begin global
#define ML_begin_global(T, value)	(_ML_GLOBALS begin_global<T>(value))

// end global
#define ML_end_global(T, value)		(_ML_GLOBALS end_global<T>(value))

// call in constructor of global
#define ML_ctor_global(T)			ML_verify(ML_begin_global(T, this))

// call in destructor of global
#define ML_dtor_global(T)			ML_verify(ML_is_global(T, this)); \
									ML_defer(&) { ML_verify(ML_end_global(T, this)); }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::globals
{
	// get global
	template <class T> ML_NODISCARD T * get_global()
	{
		static_assert(!"get global not implemented for type");
		return nullptr;
	}

	// set global
	template <class T> ML_NODISCARD T * set_global(T *)
	{
		static_assert(!"set global not implemented for type");
		return nullptr;
	}

	// is global
	template <class T> ML_NODISCARD bool is_global(T * value)
	{
		return ML_get_global(T) == value;
	}

	// begin global
	template <class T> ML_NODISCARD bool begin_global(T * value)
	{
		return value && ML_is_global(T, nullptr) && ML_set_global(T, value);
	}

	// end global
	template <class T> ML_NODISCARD bool end_global(T * value)
	{
		return value && ML_is_global(T, value) && !ML_set_global(T, nullptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_GLOBALS_HPP_