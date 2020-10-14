#ifndef _ML_CLIENT_EVENTS_HPP_
#define _ML_CLIENT_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

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
	ML_decl_event(client_dockspace_event)
	{
		client_dockspace * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_dockspace_event(client_dockspace * const ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT MENUBAR
	ML_decl_event(client_menubar_event)
	{
		client_menubar * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_menubar_event(client_menubar * const ptr) noexcept : ptr{ ptr } {}
	};

	// CLIENT GUI
	ML_decl_event(client_gui_event)
	{
		client_runtime * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_gui_event(client_runtime * const ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CLIENT_EVENTS_HPP_