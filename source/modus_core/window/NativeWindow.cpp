#include <modus_core/window/NativeWindow.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Window.hpp>
#endif

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Window.hpp>
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	native_window::native_window(allocator_type alloc) noexcept
		: m_impl{ ML_new(glfw_window, alloc) }
	{
	}

	native_window::~native_window() noexcept
	{
		ML_delete(m_impl.release());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool native_window::open(
		string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints
	)
	{
		// open base
		if (!m_impl->open(title, vm, cs, hints))
		{
			return debug::fail("failed opening backend window");
		}

		// make current context
		window_api::set_active_window(get_handle());

		// user pointer
		set_user_pointer(this);
		
		// centered
		set_position((video_mode::desktop_mode.resolution - vm.resolution) / 2);

		// maximized
		if (has_hints(window_hints_maximized)) { maximize(); }

		// success
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}