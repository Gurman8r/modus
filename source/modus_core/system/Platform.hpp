#ifndef _ML_PLATFORM_HPP_
#define _ML_PLATFORM_HPP_

#include <modus_core/system/Memory.hpp>

namespace ml
{
	ML_decl_handle(cursor_handle)	; // cursor handle
	ML_decl_handle(library_handle)	; // library handle
	ML_decl_handle(monitor_handle)	; // monitor handle
	ML_decl_handle(window_handle)	; // window handle
	
	// platform api
	class ML_CORE_API platform final
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

		static cursor_handle create_custom_cursor(size_t w, size_t h, byte const * p, int32 x, int32 y);

		static cursor_handle create_standard_cursor(int32 shape);

		static void destroy_cursor(cursor_handle const & value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLATFORM_HPP_