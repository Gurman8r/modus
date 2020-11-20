#ifndef _ML_GLOBALS_HPP_
#define _ML_GLOBALS_HPP_

#include <modus_core/Standard.hpp>
#include <modus_core/Export.hpp>

#define _ML_GLOBALS _ML globals:: // globals

// globals api
#ifndef ML_GLOBALS_API
#define ML_GLOBALS_API ML_CORE_API
#endif

// globals declaration ( header.hpp )
#define ML_decl_global(Type) template <> ML_NODISCARD ML_GLOBALS_API Type *

// globals implementation ( source.cpp )
#define ML_impl_global(Type) template <> Type *

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace globals
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

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// begin global
	template <class T> bool begin_singleton(void * value) noexcept
	{
		return value
			&& _ML get_global<T>() == nullptr
			&& _ML set_global<T>(value);
	}

	// end global
	template <class T> bool end_singleton(void * value) noexcept
	{
		return value
			&& _ML get_global<T>() == value
			&& !_ML set_global<T>(nullptr);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_GLOBALS_HPP_