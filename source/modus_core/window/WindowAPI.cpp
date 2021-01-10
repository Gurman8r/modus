#include <modus_core/window/WindowAPI.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
#endif

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Platform.hpp>
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool window_api::initialize()
	{
		return glfw_platform::initialize();
	}

	bool window_api::finalize()
	{
		return glfw_platform::finalize();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle window_api::get_active_window()
	{
		return glfw_platform::get_active_window();
	}

	window_handle window_api::set_active_window(window_handle handle)
	{
		return glfw_platform::set_active_window(handle);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void window_api::poll_events()
	{
		glfw_platform::poll_events();
	}

	void window_api::swap_buffers(window_handle handle)
	{
		glfw_platform::swap_buffers(handle);
	}

	void window_api::swap_interval(int32 value)
	{
		glfw_platform::swap_interval(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	cursor_handle window_api::create_custom_cursor(size_t w, size_t h, byte const * p, int32 x, int32 y)
	{
		return glfw_platform::create_custom_cursor(w, h, p, x, y);
	}

	cursor_handle window_api::create_standard_cursor(int32 shape)
	{
		return glfw_platform::create_standard_cursor(shape);
	}

	void window_api::destroy_cursor(cursor_handle const & handle)
	{
		glfw_platform::destroy_cursor(handle);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> window_api::get_open_file_name(window_handle handle, string const & filter)
	{
		return win32_platform::get_open_file_name(handle, filter);
	}

	std::optional<fs::path> window_api::get_save_file_name(window_handle handle, string const & filter)
	{
		return win32_platform::get_save_file_name(handle, filter);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}