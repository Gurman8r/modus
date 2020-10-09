#ifndef _ML_IMGUI_EVENTS_HPP_
#define _ML_IMGUI_EVENTS_HPP_

#include <core/system/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct imgui_context;
	struct imgui_context::main_menu_bar;
	struct imgui_context::dockspace;
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// IMGUI DOCKSPACE
	ML_decl_event(imgui_dockspace_event)
	{
		imgui_context::dockspace * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_dockspace_event(imgui_context::dockspace * const ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI MAIN MENU BAR
	ML_decl_event(imgui_menubar_event)
	{
		imgui_context::main_menu_bar * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_menubar_event(imgui_context::main_menu_bar * const ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI RENDER
	ML_decl_event(imgui_render_event)
	{
		imgui_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_render_event(imgui_context * const ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_IMGUI_EVENTS_HPP_