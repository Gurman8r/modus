#ifndef _ML_IMGUI_EVENTS_HPP_
#define _ML_IMGUI_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

struct ImGuiContext;

namespace ml::ImGuiExt { struct Dockspace; }

namespace ml
{
	// IMGUI DOCKER
	ML_decl_event(imgui_docker_event)
	{
		ImGuiExt::Dockspace * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_docker_event(ImGuiExt::Dockspace * const ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI RENDER
	ML_decl_event(imgui_render_event)
	{
		ImGuiContext * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_render_event(ImGuiContext * const ptr) noexcept : ptr{ ptr } {}
	};
}

#endif // !_ML_IMGUI_EVENTS_HPP_