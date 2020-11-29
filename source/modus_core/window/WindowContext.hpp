#ifndef _ML_WINDOW_CONTEXT_HPP_
#define _ML_WINDOW_CONTEXT_HPP_

#include <modus_core/window/WindowAPI.hpp>

namespace ml
{
	// window context (WIP)
	struct ML_CORE_API window_context final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		window_context() noexcept { ML_assert(initialize()); }

		~window_context() noexcept override { finalize(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static int32 initialize() noexcept;

		static void finalize() noexcept;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static int32 extension_supported(ds::string const & value) noexcept;

		ML_NODISCARD static void * get_proc_address(ds::string const & value) noexcept;

		ML_NODISCARD static duration get_time() noexcept;

		static window_error_callback set_error_callback(window_error_callback value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static window_handle get_active_window() noexcept;

		static void set_active_window(window_handle value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void poll_events() noexcept;

		static void swap_buffers(window_handle value) noexcept;

		static void set_swap_interval(int32 value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_WINDOW_CONTEXT_HPP_