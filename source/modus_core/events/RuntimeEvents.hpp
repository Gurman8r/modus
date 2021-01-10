#ifndef _ML_RUNTIME_EVENTS_HPP_
#define _ML_RUNTIME_EVENTS_HPP_

#include <modus_core/system/EventSystem.hpp>
#include <modus_core/detail/Duration.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct gui_application;
	
	namespace ImGuiExt { struct Dockspace; }

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(runtime_startup_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_startup_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(runtime_shutdown_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_shutdown_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(runtime_begin_frame_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_begin_frame_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(runtime_idle_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_idle_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(runtime_gui_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_gui_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(runtime_end_frame_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_end_frame_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_