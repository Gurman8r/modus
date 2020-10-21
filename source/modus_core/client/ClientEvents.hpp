#ifndef _ML_CLIENT_EVENTS_HPP_
#define _ML_CLIENT_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

struct ImGuiContext;

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct client_runtime;
	struct client_menubar;
	struct client_dockspace;

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
	ML_decl_event(client_dock_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_dock_event(client_runtime * const ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT MENUBAR
	ML_decl_event(client_menu_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_menu_event(client_runtime * const ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT GUI
	ML_decl_event(client_imgui_event)
	{
		ImGuiContext * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_imgui_event(ImGuiContext * const ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CLIENT_EVENTS_HPP_