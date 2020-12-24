#ifndef _ML_PLATFORM_HPP_
#define _ML_PLATFORM_HPP_

#include <modus_core/detail/Timer.hpp>
#include <modus_core/detail/Memory.hpp>

// HANDLES
namespace ml
{
	ML_decl_handle(cursor_handle)	; // cursor handle
	ML_decl_handle(library_handle)	; // library handle
	ML_decl_handle(monitor_handle)	; // monitor handle
	ML_decl_handle(window_handle)	; // window handle
	
	ML_alias error_callback = void(*)(int32, cstring); // static error callback
}

// PLATFORM
namespace ml
{
	// platform
	class ML_CORE_API platform final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool initialize();

		static bool finalize();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static error_callback get_error_callback();

		static error_callback set_error_callback(error_callback value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static library_handle load_library(fs::path const & path);

		static bool free_library(library_handle instance);

		static void * get_proc_address(library_handle instance, ds::string const & name);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static window_handle get_context_current();

		static void make_context_current(window_handle value);

		static void poll_events();

		static void swap_buffers(window_handle window);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static std::optional<fs::path> get_open_file_name(window_handle window, ds::string const & filter);

		static std::optional<fs::path> get_save_file_name(window_handle window, ds::string const & filter);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLATFORM_HPP_