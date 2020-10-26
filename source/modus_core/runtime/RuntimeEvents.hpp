#ifndef _ML_CLIENT_EVENTS_HPP_
#define _ML_CLIENT_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// ENTER
	ML_decl_event(client_enter_event)
	{
		struct runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_enter_event(struct runtime_context * ptr) noexcept : ptr{ ptr } {}
	};

	// EXIT
	ML_decl_event(client_exit_event)
	{
		struct runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_exit_event(struct runtime_context * ptr) noexcept : ptr{ ptr } {}
	};

	// IDLE
	ML_decl_event(client_idle_event)
	{
		struct runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr client_idle_event(struct runtime_context * ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI DOCKER
	ML_decl_event(imgui_docker_event)
	{
		struct runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_docker_event(struct runtime_context * const ptr) noexcept : ptr{ ptr } {}
	};

	// IMGUI RENDER
	ML_decl_event(imgui_render_event)
	{
		struct runtime_context * const ptr;
		auto operator->() const noexcept { return ptr; }
		auto & operator*() const noexcept { return *ptr; }
		constexpr imgui_render_event(struct runtime_context * const ptr) noexcept : ptr{ ptr } {}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CLIENT_EVENTS_HPP_