#ifndef _ML_IMGUI_EVENTS_HPP_
#define _ML_IMGUI_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>

struct ImGuiContext;

namespace ml::ImGuiExt { struct Dockspace; struct MainMenuBar; }

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// DOCK BUILDER
	ML_event(dock_builder_event)
	{
		ImGuiExt::Dockspace * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr dock_builder_event(ImGuiExt::Dockspace * ptr) noexcept : ptr{ ptr } {}
	};

	// MAIN MENU BAR
	ML_event(main_menu_bar_event)
	{
		ImGuiExt::MainMenuBar * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr main_menu_bar_event(ImGuiExt::MainMenuBar * ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI RENDER
	ML_event(imgui_event)
	{
		ImGuiContext * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_event(ImGuiContext * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_IMGUI_EVENTS_HPP_