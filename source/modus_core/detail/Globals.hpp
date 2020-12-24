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

namespace ml::globals
{
	// backend global getter
	template <class T> ML_NODISCARD T * get() noexcept
	{
		static_assert(0, "get global not implemented for type");
		return nullptr;
	}

	// backend global setter
	template <class T> ML_NODISCARD T * set(T *) noexcept
	{
		static_assert(0, "set global not implemented for type");
		return nullptr;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	// get global macro
#define ML_global(T) _ML get_global<T>()

	// get global
	template <class T> ML_NODISCARD auto get_global() noexcept
	{
		static_assert(!std::is_same_v<T, void>, "?");

		return _ML_GLOBALS get<T>();
	}

	// set global
	template <class T> auto set_global(void * value) noexcept
	{
		static_assert(!std::is_same_v<T, void>, "?");
		
		return _ML_GLOBALS set<T>(static_cast<T *>(value));
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	// get singleton macro (checked global)
#define ML_singleton(T) ((T *)ML_check(ML_global(T)))

	// begin singleton
	template <class T> ML_NODISCARD bool begin_singleton(void * value) noexcept
	{
		return value
			&& _ML get_global<T>() == nullptr
			&& _ML set_global<T>(value);
	}

	// end singleton
	template <class T> ML_NODISCARD bool end_singleton(void * value) noexcept
	{
		return value
			&& _ML get_global<T>() == value
			&& !_ML set_global<T>(nullptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_GLOBALS_HPP_