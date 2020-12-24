#ifndef _ML_WIN32_PLATFORM_HPP_
#define _ML_WIN32_PLATFORM_HPP_

#include <modus_core/system/Platform.hpp>

static_assert(ML_os_windows);

namespace ml
{
	// windows platform api
	class win32_platform final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static library_handle load_library(fs::path const & path);

		static bool free_library(library_handle instance);

		static void * get_proc_address(library_handle instance, ds::string const & name);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static std::optional<fs::path> get_open_file_name(window_handle window, ds::string const & filter);

		static std::optional<fs::path> get_save_file_name(window_handle window, ds::string const & filter);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // _ML_WIN32_PLATFORM_HPP_