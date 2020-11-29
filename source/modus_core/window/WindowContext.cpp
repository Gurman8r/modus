#include <modus_core/window/WindowContext.hpp>

#if defined(ML_IMPL_WINDOW_GLFW)
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using impl_context = _ML glfw_context;

#else
#error "window_context unavailable"
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32 window_context::initialize() noexcept
	{
		return impl_context::initialize();
	}

	void window_context::finalize() noexcept
	{
		impl_context::finalize();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32 window_context::extension_supported(ds::string const & value) noexcept
	{
		return impl_context::extension_supported(value.c_str());
	}

	void * window_context::get_proc_address(ds::string const & value) noexcept
	{
		return impl_context::get_proc_address(value.c_str());
	}

	duration window_context::get_time() noexcept
	{
		return impl_context::get_time();
	}

	window_error_callback window_context::set_error_callback(window_error_callback value) noexcept
	{
		return impl_context::set_error_callback(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle window_context::get_active_window() noexcept
	{
		return impl_context::get_active_window();
	}

	void window_context::set_active_window(window_handle value) noexcept
	{
		impl_context::set_active_window(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void window_context::poll_events() noexcept
	{
		impl_context::poll_events();
	}

	void window_context::swap_buffers(window_handle value) noexcept
	{
		impl_context::swap_buffers(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void window_context::set_swap_interval(int32 value) noexcept
	{
		impl_context::set_swap_interval(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}