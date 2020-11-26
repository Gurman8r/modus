#include <modus_core/window/NativeWindow.hpp>

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using impl_window	= _ML glfw_window;
using impl_context	= _ML glfw_context;
using impl_cursor	= _ML glfw_cursor;

#else
#error "native window unavailable"
#endif

namespace ml
{
	ML_NODISCARD auto make_window_backend(pmr::polymorphic_allocator<byte> alloc) noexcept
	{
		return ::new (alloc.allocate(sizeof(impl_window))) impl_window{ alloc };
	}
}

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	native_window::native_window(allocator_type alloc) noexcept
		: frontend_window{ make_window_backend(alloc) }
	{
	}

	native_window::native_window(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		allocator_type				alloc
	) noexcept : native_window{ alloc }
	{
		ML_assert(this->open(title, vm, cs, hints));
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
		if (!frontend_window::open(title, vm, cs, hints)) { return false; }
		
		// make current context
		set_active_window(get_handle());

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

	int32 native_window::initialize() noexcept
	{
		return impl_context::initialize();
	}

	void native_window::finalize() noexcept
	{
		impl_context::finalize();
	}

	void native_window::poll_events() noexcept
	{
		impl_context::poll_events();
	}

	void native_window::swap_buffers(window_handle value) noexcept
	{
		impl_context::swap_buffers(value);
	}

	int32 native_window::extension_supported(ds::string const & value) noexcept
	{
		return impl_context::extension_supported(value.c_str());
	}

	window_handle native_window::get_active_window() noexcept
	{
		return impl_context::get_active_window();
	}

	ds::list<monitor_handle> const & native_window::get_monitors() noexcept
	{
		return impl_context::get_monitors();
	}

	monitor_handle native_window::get_primary_monitor() noexcept
	{
		return impl_context::get_primary_monitor();
	}

	void * native_window::get_proc_address(ds::string const & value) noexcept
	{
		return impl_context::get_proc_address(value.c_str());
	}

	duration native_window::get_time() noexcept
	{
		return impl_context::get_time();
	}

	void native_window::set_active_window(window_handle value) noexcept
	{
		impl_context::set_active_window(value);
	}

	window_error_callback native_window::set_error_callback(window_error_callback value) noexcept
	{
		return impl_context::set_error_callback(value);
	}

	void native_window::set_swap_interval(int32 value) noexcept
	{
		impl_context::set_swap_interval(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	cursor_handle native_window::create_custom_cursor(size_t w, size_t h, byte const * p, int32 x, int32 y) noexcept
	{
		return impl_cursor::create_custom_cursor(w, h, p, x, y);
	}

	cursor_handle native_window::create_standard_cursor(cursor_shape_ value) noexcept
	{
		return impl_cursor::create_standard_cursor(value);
	}

	void native_window::destroy_cursor(cursor_handle value) noexcept
	{
		impl_cursor::destroy_cursor(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}