#ifndef _ML_IMGUI_EVENTS_HPP_
#define _ML_IMGUI_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct imgui_runtime;
	struct imgui_runtime::menubar;
	struct imgui_runtime::dockspace;
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// IMGUI DOCKSPACE
	ML_decl_event(imgui_dockspace_event)
	{
		imgui_runtime::dockspace * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_dockspace_event(imgui_runtime::dockspace * const ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI MAIN MENU BAR
	ML_decl_event(imgui_menubar_event)
	{
		imgui_runtime::menubar * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_menubar_event(imgui_runtime::menubar * const ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI RENDER
	ML_decl_event(imgui_render_event)
	{
		imgui_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_render_event(imgui_runtime * const ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_IMGUI_EVENTS_HPP_