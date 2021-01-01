#ifndef _ML_RUNTIME_EVENTS_HPP_
#define _ML_RUNTIME_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/detail/Duration.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct gui_application;
	
	namespace ImGuiExt { struct Dockspace; }

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(setup_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr setup_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(cleanup_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr cleanup_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(update_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr update_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(late_update_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr late_update_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(dockspace_event)
	{
		ImGuiExt::Dockspace * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr dockspace_event(ImGuiExt::Dockspace * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(gui_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr gui_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(gizmos_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr gizmos_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	ML_event(end_frame_event)
	{
		gui_application * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr end_frame_event(gui_application * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RUNTIME_EVENTS_HPP_