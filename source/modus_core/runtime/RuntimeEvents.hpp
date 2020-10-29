#ifndef _ML_RUNTIME_EVENTS_HPP_
#define _ML_RUNTIME_EVENTS_HPP_

#include <modus_core/detail/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// PROCESS ENTER
	ML_decl_event(main_enter_event)
	{
		struct player_loop * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr main_enter_event(struct player_loop * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// PROCESS EXIT
	ML_decl_event(main_exit_event)
	{
		struct player_loop * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr main_exit_event(struct player_loop * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// PROCESS IDLE
	ML_decl_event(main_idle_event)
	{
		struct player_loop * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr main_idle_event(struct player_loop * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_