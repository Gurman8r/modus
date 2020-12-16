#ifndef _ML_IMGUI_EVENTS_HPP_
#define _ML_IMGUI_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>

struct ImGuiContext;

namespace ml::ImGuiExt { struct Dockspace; struct MainMenuBar; }

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// IMGUI BEGIN
	ML_event(imgui_begin_event)
	{
		ImGuiContext * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr imgui_begin_event(ImGuiContext * const p) noexcept : p{ p } {}
	};

	// IMGUI RENDER
	ML_event(imgui_render_event)
	{
		ImGuiContext * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr imgui_render_event(ImGuiContext * const p) noexcept : p{ p } {}
	};

	// IMGUI END
	ML_event(imgui_end_event)
	{
		ImGuiContext * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr imgui_end_event(ImGuiContext * const p) noexcept : p{ p } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// DOCK BUILDER
	ML_event(dock_builder_event)
	{
		ImGuiExt::Dockspace * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr dock_builder_event(ImGuiExt::Dockspace * const p) noexcept : p{ p } {}
	};

	// MAIN MENU BAR
	ML_event(main_menu_bar_event)
	{
		ImGuiExt::MainMenuBar * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr main_menu_bar_event(ImGuiExt::MainMenuBar * const p) noexcept : p{ p } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_IMGUI_EVENTS_HPP_