#ifndef _ML_GLOBALS_HPP_
#define _ML_GLOBALS_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/Common.hpp>
#include <modus_core/Export.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// globals namespace
#define _ML_GLOBALS _ML globals::

// globals api
#ifndef ML_GLOBAL_API
#define ML_GLOBAL_API ML_CORE_API
#endif

// globals declaration
#define ML_decl_global(Type) template <> ML_NODISCARD ML_GLOBAL_API Type *

// globals implementation
#define ML_impl_global(Type) template <> Type *

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::globals
{
	// backend get global
	template <class T> ML_NODISCARD T * get() noexcept
	{
		static_assert(0, "get global not implemented for type");
		return nullptr;
	}

	// backend set global
	template <class T> ML_NODISCARD T * set(T *) noexcept
	{
		static_assert(0, "set global not implemented for type");
		return nullptr;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// globals
namespace ml
{
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
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_GLOBALS_HPP_