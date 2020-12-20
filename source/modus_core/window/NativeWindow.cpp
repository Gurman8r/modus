#include <modus_core/window/NativeWindow.hpp>

#if defined(ML_IMPL_WINDOW_GLFW)
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using impl_window = _ML glfw_window;

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
			return debug::failure("failed opening backend window");
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

	std::optional<fs::path> native_window::get_open_file_name(cstring filter) const
	{
		return is_open()
			? platform::get_open_file_name(get_native_handle(), filter)
			: std::nullopt;
	}

	std::optional<fs::path> native_window::get_save_file_name(cstring filter) const
	{
		return is_open()
			? platform::get_save_file_name(get_native_handle(), filter)
			: std::nullopt;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}