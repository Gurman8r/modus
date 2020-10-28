#ifndef _ML_RUNTIME_EVENTS_HPP_
#define _ML_RUNTIME_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

namespace ml { struct runtime_context; }

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// RUNTIME ENTER
	ML_decl_event(runtime_enter_event)
	{
		runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr runtime_enter_event(runtime_context * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// RUNTIME EXIT
	ML_decl_event(runtime_exit_event)
	{
		runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr runtime_exit_event(runtime_context * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// RUNTIME IDLE
	ML_decl_event(runtime_idle_event)
	{
		runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr runtime_idle_event(runtime_context * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_