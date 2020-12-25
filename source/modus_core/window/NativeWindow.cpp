#include <modus_core/window/NativeWindow.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
#endif

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Window.hpp>
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	native_window::native_window(allocator_type alloc) noexcept
		: frontend_window{ ::new (alloc.allocate(sizeof(glfw_window))) glfw_window{ alloc } }
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
		glfw_platform::make_context_current(get_handle());

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

	bool native_window::initialize() noexcept
	{
		return glfw_platform::initialize();
	}

	bool native_window::finalize() noexcept
	{
		return glfw_platform::finalize();
	}

	window_handle native_window::get_context_current() noexcept
	{
		return glfw_platform::get_context_current();
	}

	void native_window::make_context_current(window_handle value) noexcept
	{
		glfw_platform::make_context_current(value);
	}

	void native_window::poll_events() noexcept
	{
		glfw_platform::poll_events();
	}

	void native_window::swap_buffers(window_handle value) noexcept
	{
		glfw_platform::swap_buffers(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> native_window::get_open_file_name(ds::string const & filter) const
	{
		return is_open()
			? win32_platform::get_open_file_name(get_native_handle(), filter)
			: std::nullopt;
	}

	std::optional<fs::path> native_window::get_save_file_name(ds::string const & filter) const
	{
		return is_open()
			? win32_platform::get_save_file_name(get_native_handle(), filter)
			: std::nullopt;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}