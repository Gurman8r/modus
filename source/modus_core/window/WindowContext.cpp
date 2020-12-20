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

	int32 window_context::initialize()
	{
		return impl_context::initialize();
	}

	void window_context::finalize()
	{
		impl_context::finalize();
	}

	window_error_callback window_context::set_error_callback(window_error_callback value)
	{
		return impl_context::set_error_callback(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle window_context::get_active_window()
	{
		return impl_context::get_active_window();
	}

	void window_context::set_active_window(window_handle value)
	{
		impl_context::set_active_window(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void window_context::poll_events()
	{
		impl_context::poll_events();
	}

	void window_context::swap_buffers(window_handle value)
	{
		impl_context::swap_buffers(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void window_context::set_swap_interval(int32 value)
	{
		impl_context::set_swap_interval(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}