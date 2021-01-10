#ifndef _ML_WIN32_PLATFORM_HPP_
#define _ML_WIN32_PLATFORM_HPP_

#include <modus_core/embed/Library.hpp>
#include <modus_core/window/BaseWindow.hpp>

static_assert(ML_os_windows);

namespace ml
{
	// windows platform api
	class win32_platform final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool initialize();

		static bool finalize();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static library_handle load_library(fs::path const & path);

		static bool free_library(library_handle handle);

		static void * get_proc_address(library_handle handle, string const & method_name);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static window_handle get_active_window();

		static window_handle set_active_window(window_handle handle);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void poll_events();

		static void swap_buffers(window_handle handle);

		static void swap_interval(int32 value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static std::optional<fs::path> get_open_file_name(window_handle handle, string const & filter = "");

		static std::optional<fs::path> get_save_file_name(window_handle handle, string const & filter = "");

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // _ML_WIN32_PLATFORM_HPP_