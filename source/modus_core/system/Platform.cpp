#include <modus_core/system/Platform.hpp>

#if defined(ML_os_windows)
#include <modus_core/backends/win32/Win32_Platform.hpp>
using impl_platform = _ML win32_platform;

#else
#error "platform unavailable"
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	library_handle platform::load_library(fs::path const & path)
	{
		return impl_platform::load_library(path);
	}

	bool platform::free_library(library_handle instance)
	{
		return impl_platform::free_library(instance);
	}

	void * platform::get_proc_address(library_handle instance, ds::string const & name)
	{
		return impl_platform::get_proc_address(instance, name);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> platform::get_open_file_name(window_handle window, ds::string const & filter)
	{
		return impl_platform::get_open_file_name(window, filter);
	}

	std::optional<fs::path> platform::get_save_file_name(window_handle window, ds::string const & filter)
	{
		return impl_platform::get_save_file_name(window, filter);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}