#ifndef _ML_NATIVE_WINDOW_HPP_
#define _ML_NATIVE_WINDOW_HPP_

#include <modus_core/window/BaseWindow.hpp>

namespace ml
{
	// native window
	struct ML_CORE_API native_window : frontend_window<native_window>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using frontend_window::allocator_type;

		native_window(allocator_type alloc = {}) noexcept;

		native_window(
			ds::string			const & title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			allocator_type				alloc	= {}) noexcept;
		
		virtual ~native_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			ds::string			const &	title,
			video_mode			const &	vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default
		) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static int32 initialize() noexcept;

		static void finalize() noexcept;

		static void poll_events() noexcept;

		static void swap_buffers(window_handle value) noexcept;

		ML_NODISCARD static int32 extension_supported(ds::string const & value) noexcept;

		ML_NODISCARD static window_handle get_active_window() noexcept;

		ML_NODISCARD static ds::list<monitor_handle> const & get_monitors() noexcept;

		ML_NODISCARD static monitor_handle get_primary_monitor() noexcept;

		ML_NODISCARD static void * get_proc_address(ds::string const & value) noexcept;
		
		ML_NODISCARD static duration get_time() noexcept;

		ML_NODISCARD static void * get_user_pointer(window_handle value) noexcept;

		static void set_active_window(window_handle value) noexcept;

		static window_error_callback set_error_callback(window_error_callback value) noexcept;

		static void set_swap_interval(int32 value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static cursor_handle create_custom_cursor(size_t w, size_t h, byte const * p, int32 x, int32 y) noexcept;
		
		ML_NODISCARD static cursor_handle create_standard_cursor(cursor_shape_ value) noexcept;

		static void destroy_cursor(cursor_handle value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NATIVE_WINDOW_HPP_