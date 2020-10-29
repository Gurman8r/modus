#ifndef _ML_RUNTIME_EVENTS_HPP_
#define _ML_RUNTIME_EVENTS_HPP_

#include <modus_core/detail/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// PROCESS ENTER
	ML_decl_event(process_enter_event)
	{
		struct main_loop * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr process_enter_event(struct main_loop * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// PROCESS EXIT
	ML_decl_event(process_exit_event)
	{
		struct main_loop * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr process_exit_event(struct main_loop * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// PROCESS IDLE
	ML_decl_event(process_idle_event)
	{
		struct main_loop * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr process_idle_event(struct main_loop * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_