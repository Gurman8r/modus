#ifndef _ML_GLOBALS_HPP_
#define _ML_GLOBALS_HPP_

#include <modus_core/Common.hpp>
#include <modus_core/Export.hpp>

// globals namespace
#define _ML_GLOBALS _ML globals::

// globals api
#ifndef ML_GLOBALS_API
#define ML_GLOBALS_API ML_CORE_API
#endif

// globals declaration ( in .hpp )
#define ML_decl_global(T) template <> ML_NODISCARD ML_GLOBALS_API T *

// globals implementation ( in .cpp )
#define ML_impl_global(T) template <> T *

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace globals
	{
		// backend global get
		template <class T> ML_NODISCARD T * get() noexcept
		{
			static_assert(0, "get global not implemented for type");
			return nullptr;
		}

		// backend global set
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
		using U = std::_Remove_cvref_t<T>;
		
		static_assert(!std::is_same_v<U, void>, "?");
		
		return _ML_GLOBALS get<U>();
	}

	// set global
	template <class T> auto set_global(void * value) noexcept
	{
		using U = std::_Remove_cvref_t<T>;
		
		static_assert(!std::is_same_v<U, void>, "?");
		
		return _ML_GLOBALS set<U>(static_cast<U *>(value));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_GLOBALS_HPP_