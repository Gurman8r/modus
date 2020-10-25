#ifndef _ML_CLIENT_EVENTS_HPP_
#define _ML_CLIENT_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

struct ImGuiContext;

namespace ml { struct client_runtime; }

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// CLIENT INIT
	ML_decl_event(client_enter_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_enter_event(client_runtime * ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT EXIT
	ML_decl_event(client_exit_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_exit_event(client_runtime * ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT IDLE
	ML_decl_event(client_idle_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_idle_event(client_runtime * ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT DOCKSPACE
	ML_decl_event(imgui_docker_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_docker_event(client_runtime * const ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT GUI
	ML_decl_event(imgui_render_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_render_event(client_runtime * const ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CLIENT_EVENTS_HPP_