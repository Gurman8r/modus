#ifndef _ML_RUNTIME_EVENTS_HPP_
#define _ML_RUNTIME_EVENTS_HPP_

#include <modus_core/detail/Events.hpp>

namespace ml { struct application; }

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// APPLICATION ENTER
	ML_event(app_enter_event)
	{
		application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr app_enter_event(application * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// APPLICATION EXIT
	ML_event(app_exit_event)
	{
		application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr app_exit_event(application * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// APPLICATION IDLE
	ML_event(app_idle_event)
	{
		application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr app_idle_event(application * ptr) noexcept : ptr{ ptr }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_