#include <modus_core/runtime/SharedLibrary.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
#endif // ML_os_windows

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	library_handle shared_library::load_library(fs::path const & path)
	{
		return win32_platform::load_library(path);
	}

	bool shared_library::free_library(library_handle instance)
	{
		return win32_platform::free_library(instance);
	}

	void * shared_library::get_proc_address(library_handle instance, ds::string const & name)
	{
		return win32_platform::get_proc_address(instance, name);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}