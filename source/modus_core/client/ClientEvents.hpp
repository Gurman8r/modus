#ifndef _ML_CLIENT_EVENTS_HPP_
#define _ML_CLIENT_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct client_context;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// LOAD
	ML_decl_event(client_init_event)
	{
		client_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_init_event(client_context * ptr) noexcept : ptr{ ptr } {}
	};

	// UNLOAD
	ML_decl_event(client_exit_event)
	{
		client_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_exit_event(client_context * ptr) noexcept : ptr{ ptr } {}
	};

	// UPDATE
	ML_decl_event(client_idle_event)
	{
		client_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_idle_event(client_context * ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CLIENT_EVENTS_HPP_