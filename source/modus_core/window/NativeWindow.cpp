#include <modus_core/window/NativeWindow.hpp>

#if defined(ML_IMPL_WINDOW_GLFW)
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using impl_window	= _ML glfw_window;
using impl_context	= _ML glfw_context;
using impl_cursor	= _ML glfw_cursor;

#else
#error "native_window unavailable"
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	native_window::native_window(allocator_type alloc) noexcept
		: frontend_window{ ::new (alloc.allocate(sizeof(impl_window))) impl_window{ alloc } }
	{
	}

	native_window::~native_window() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool native_window::open(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints
	)
	{
		// open base
		if (!frontend_window::open(title, vm, cs, hints)) {
			return debug::failure("failed opening frontend_window");
		}

		// make current context
		window_context::set_active_window(get_handle());

		// user pointer
		set_user_pointer(this);
		
		// centered
		set_position((video_mode::get_desktop_mode().resolution - vm.resolution) / 2);

		// maximized
		if (has_hints(window_hints_maximized)) { maximize(); }

		// success
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}