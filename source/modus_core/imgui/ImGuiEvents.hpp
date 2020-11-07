#ifndef _ML_IMGUI_EVENTS_HPP_
#define _ML_IMGUI_EVENTS_HPP_

#include <modus_core/detail/Events.hpp>

struct ImGuiContext;

namespace ml::ImGuiExt { struct Dockspace; }

namespace ml
{
	// IMGUI DOCKSPACE
	ML_event(imgui_dockspace_event)
	{
		ImGuiExt::Dockspace * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_dockspace_event(ImGuiExt::Dockspace * const ptr) noexcept : ptr{ ptr }
		{
		}
	};

	// IMGUI RENDER
	ML_event(imgui_render_event)
	{
		ImGuiContext * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_render_event(ImGuiContext * const ptr) noexcept : ptr{ ptr }
		{
		}
	};
}

#endif // !_ML_IMGUI_EVENTS_HPP_