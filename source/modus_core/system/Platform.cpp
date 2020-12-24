#include <modus_core/system/Platform.hpp>

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Window.hpp>
#endif

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool platform::initialize()
	{
		return glfw_platform::initialize();
	}

	bool platform::finalize()
	{
		return glfw_platform::finalize();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	error_callback platform::get_error_callback()
	{
		return glfw_platform::get_error_callback();
	}

	error_callback platform::set_error_callback(error_callback value)
	{
		return glfw_platform::set_error_callback(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	library_handle platform::load_library(fs::path const & path)
	{
		return win32_platform::load_library(path);
	}

	bool platform::free_library(library_handle instance)
	{
		return win32_platform::free_library(instance);
	}

	void * platform::get_proc_address(library_handle instance, ds::string const & name)
	{
		return win32_platform::get_proc_address(instance, name);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle platform::get_context_current()
	{
		return glfw_platform::get_context_current();
	}

	void platform::make_context_current(window_handle value)
	{
		glfw_platform::make_context_current(value);
	}

	void platform::poll_events()
	{
		glfw_platform::poll_events();
	}

	void platform::swap_buffers(window_handle window)
	{
		glfw_platform::swap_buffers(window);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> platform::get_open_file_name(window_handle window, ds::string const & filter)
	{
		return win32_platform::get_open_file_name(window, filter);
	}

	std::optional<fs::path> platform::get_save_file_name(window_handle window, ds::string const & filter)
	{
		return win32_platform::get_save_file_name(window, filter);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}