#ifndef _ML_GLOBALS_HPP_
#define _ML_GLOBALS_HPP_

#include <modus_core/Common.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace globals
	{
		// get global implementation
		template <class T> T * get() noexcept
		{
			static_assert(0, "get global not implemented");
			return nullptr;
		}

		// set global implementation
		template <class T> T * set(T *) noexcept
		{
			static_assert(0, "set global not implemented");
			return nullptr;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get global
	template <class T> ML_NODISCARD auto get_global() noexcept
	{
		using U = std::_Remove_cvref_t<T>;
		static_assert(!std::is_same_v<U, void>, "?");
		return _ML globals::get<U>();
	}

	// set global
	template <class T> auto set_global(void * value) noexcept
	{
		using U = std::_Remove_cvref_t<T>;
		static_assert(!std::is_same_v<U, void>, "?");
		return _ML globals::set<U>(static_cast<U *>(value));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_GLOBALS_HPP_