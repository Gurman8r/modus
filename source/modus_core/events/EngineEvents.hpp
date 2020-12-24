#ifndef _ML_ENGINE_EVENTS_HPP_
#define _ML_ENGINE_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/detail/Duration.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct gui_application;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(load_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr load_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(unload_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr unload_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(begin_step_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr begin_step_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(end_step_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr end_step_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_ENGINE_EVENTS_HPP_