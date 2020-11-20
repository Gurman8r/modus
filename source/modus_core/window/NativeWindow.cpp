#include <modus_core/window/NativeWindow.hpp>

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using impl_window = _ML glfw_window;
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	native_window::native_window(allocator_type alloc) noexcept
		: frontend_window{ ::new (alloc.allocate(sizeof(impl_window))) impl_window{ alloc } }
	{
	}

	native_window::native_window(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr,
		allocator_type				alloc
	) noexcept : native_window{ alloc }
	{
		ML_assert(this->open(title, vm, cs, hints, userptr));
	}

	native_window::~native_window() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool native_window::open(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr
	)
	{
		// open base
		if (!frontend_window::open(title, vm, cs, hints, userptr)) { return false; }
		
		// make current context
		get_window_context()->set_active_window(get_handle());
		
		// centered
		set_position((video_mode::get_desktop_mode().resolution - vm.resolution) / 2);

		// maximized
		if (has_hints(window_hints_maximized)) { maximize(); }

		// success
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}