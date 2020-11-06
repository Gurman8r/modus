#ifndef _ML_RUNTIME_EVENTS_HPP_
#define _ML_RUNTIME_EVENTS_HPP_

#include <modus_core/detail/Events.hpp>

namespace ml { struct application; }

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// RUNTIME ENTER
	ML_decl_event(runtime_enter_event)
	{
		application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr runtime_enter_event(application * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// RUNTIME EXIT
	ML_decl_event(runtime_exit_event)
	{
		application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr runtime_exit_event(application * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// RUNTIME IDLE
	ML_decl_event(runtime_idle_event)
	{
		application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr runtime_idle_event(application * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_