#ifndef _ML_IMGUI_EVENTS_HPP_
#define _ML_IMGUI_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>

struct ImGuiContext;

namespace ml::ImGuiExt { struct Dockspace; struct MainMenuBar; }

namespace ml
{
	// IMGUI DOCKSPACE
	ML_event(imgui_dockspace_event)
	{
		ImGuiExt::Dockspace * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr imgui_dockspace_event(ImGuiExt::Dockspace * const p) noexcept : p{ p }
		{
		}
	};

	// IMGUI MENUBAR
	ML_event(imgui_menubar_event)
	{
		ImGuiExt::MainMenuBar * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr imgui_menubar_event(ImGuiExt::MainMenuBar * const p) noexcept : p{ p }
		{
		}
	};

	// IMGUI RENDER
	ML_event(imgui_render_event)
	{
		ImGuiContext * const p;
		auto operator->() const noexcept { return p; }
		auto & operator*() const noexcept { return *p; }
		constexpr imgui_render_event(ImGuiContext * const p) noexcept : p{ p }
		{
		}
	};
}

#endif // !_ML_IMGUI_EVENTS_HPP_