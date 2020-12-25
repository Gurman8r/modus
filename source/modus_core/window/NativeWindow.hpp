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

		using frontend_window::frontend_window;

		native_window(allocator_type alloc = {}) noexcept;

		virtual ~native_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			ds::string const &			title,
			video_mode const &			vm		= {},
			context_settings const &	cs		= {},
			window_hints_				hints	= window_hints_default
		) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static bool initialize() noexcept;

		ML_NODISCARD static bool finalize() noexcept;

		ML_NODISCARD static window_handle get_context_current() noexcept;

		static void make_context_current(window_handle value) noexcept;

		static void poll_events() noexcept;

		static void swap_buffers(window_handle value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD std::optional<fs::path> get_open_file_name(ds::string const & filter = "") const;

		ML_NODISCARD std::optional<fs::path> get_save_file_name(ds::string const & filter = "") const;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NATIVE_WINDOW_HPP_