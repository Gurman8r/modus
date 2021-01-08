#include <modus_core/system/Platform.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
#endif

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Platform.hpp>
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

	library_handle platform::load_library(fs::path const & path)
	{
		return win32_platform::load_library(path);
	}

	bool platform::free_library(library_handle handle)
	{
		return win32_platform::free_library(handle);
	}

	void * platform::get_proc_address(library_handle handle, string const & method_name)
	{
		return win32_platform::get_proc_address(handle, method_name);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle platform::get_active_window()
	{
		return glfw_platform::get_active_window();
	}

	window_handle platform::set_active_window(window_handle handle)
	{
		return glfw_platform::set_active_window(handle);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void platform::poll_events()
	{
		glfw_platform::poll_events();
	}

	void platform::swap_buffers(window_handle handle)
	{
		glfw_platform::swap_buffers(handle);
	}

	void platform::swap_interval(int32 value)
	{
		glfw_platform::swap_interval(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> platform::get_open_file_name(window_handle handle, string const & filter)
	{
		return win32_platform::get_open_file_name(handle, filter);
	}

	std::optional<fs::path> platform::get_save_file_name(window_handle handle, string const & filter)
	{
		return win32_platform::get_save_file_name(handle, filter);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	cursor_handle platform::create_custom_cursor(size_t w, size_t h, byte const * p, int32 x, int32 y)
	{
		return glfw_platform::create_custom_cursor(w, h, p, x, y);
	}

	cursor_handle platform::create_standard_cursor(int32 shape)
	{
		return glfw_platform::create_standard_cursor(shape);
	}

	void platform::destroy_cursor(cursor_handle const & handle)
	{
		glfw_platform::destroy_cursor(handle);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}