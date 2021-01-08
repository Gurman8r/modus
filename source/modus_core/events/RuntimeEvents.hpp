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

	ML_event(runtime_update_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_update_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(runtime_imgui_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_imgui_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(runtime_frame_end_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ML_check(ptr); }
		constexpr runtime_frame_end_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_