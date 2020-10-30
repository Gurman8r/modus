#include <modus_core/window/DefaultWindow.hpp>

#ifdef ML_IMPL_WINDOW_GLFW
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using native_window = _ML glfw_window;
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	default_window::default_window(base_window * backend) noexcept
		: m_backend{ ML_check(backend) }
	{
	}

	default_window::default_window(allocator_type alloc) noexcept
		: m_backend{ new native_window{ alloc } }
	{
	}

	default_window::default_window(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr,
		allocator_type				alloc
	) noexcept : default_window{ alloc }
	{
		ML_assert(this->open(title, vm, cs, hints, userptr));
	}

	default_window::~default_window() noexcept
	{
		clear_callbacks();

		set_error_callback(nullptr);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool default_window::open(
		ds::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		void *						userptr
	)
	{
		// check already open
		if (is_open()) {
			return debug::error("default_window is already open");
		}

		// open default_window
		if (!m_backend->open(title, vm, cs, hints, userptr)) {
			return debug::error("failed opening default_window");
		}
		
		// make current context
		set_active_window(get_handle());
		
		// centered
		set_position((video_mode::get_desktop_mode().resolution - vm.resolution) / 2);

		// maximized
		if (has_hints(window_hints_maximized)) { maximize(); }

		// success
		return is_open();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void default_window::focus() noexcept
	{
		m_backend->focus();
	}

	void default_window::hide() noexcept
	{
		m_backend->hide();
	}

	void default_window::iconify() noexcept
	{
		m_backend->iconify();
	}

	void default_window::maximize() noexcept
	{
		m_backend->maximize();
	}

	void default_window::restore() noexcept
	{
		m_backend->restore();
	}

	void default_window::request_attention() noexcept
	{
		m_backend->request_attention();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	default_window::allocator_type default_window::get_allocator() const noexcept
	{
		return m_backend->get_allocator();
	}

	int_rect default_window::get_bounds() const noexcept
	{
		return m_backend->get_bounds();
	}

	window_callbacks const & default_window::get_callbacks() const noexcept
	{
		return m_backend->get_callbacks();
	}

	window_context_manager const & default_window::get_context_manager() const noexcept
	{
		return m_backend->get_context_manager();
	}

	cstring default_window::get_clipboard() const noexcept
	{
		return m_backend->get_clipboard();
	}

	vec2 default_window::get_content_scale() const noexcept
	{
		return m_backend->get_content_scale();
	}

	vec2 default_window::get_cursor_pos() const noexcept
	{
		return m_backend->get_cursor_pos();
	}

	int32_t default_window::get_cursor_mode() const noexcept
	{
		return m_backend->get_cursor_mode();
	}

	vec2i default_window::get_framebuffer_size() const noexcept
	{
		return m_backend->get_framebuffer_size();
	}

	window_handle default_window::get_handle() const noexcept
	{
		return m_backend->get_handle();
	}

	window_hints_ default_window::get_hints() const noexcept
	{
		return m_backend->get_hints();
	}

	int32_t default_window::get_input_mode(int32_t value) const noexcept
	{
		return m_backend->get_input_mode(value);
	}

	int32_t	default_window::get_key(int32_t value) const noexcept
	{
		return m_backend->get_key(value);
	}

	int32_t	default_window::get_mouse_button(int32_t value) const noexcept
	{
		return m_backend->get_mouse_button(value);
	}

	window_handle default_window::get_native_handle() const noexcept
	{
		return m_backend->get_native_handle();
	}

	float_t default_window::get_opacity() const noexcept
	{
		return m_backend->get_opacity();
	}

	vec2i default_window::get_position() const noexcept
	{
		return m_backend->get_position();
	}

	vec2i default_window::get_size() const noexcept
	{
		return m_backend->get_size();
	}

	ds::string const & default_window::get_title() const noexcept
	{
		return m_backend->get_title();
	}

	void * default_window::get_user_pointer() const noexcept
	{
		return m_backend->get_user_pointer();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool default_window::is_auto_iconify() const noexcept
	{
		return m_backend->is_auto_iconify();
	}

	bool default_window::is_decorated() const noexcept
	{
		return m_backend->is_decorated();
	}

	bool default_window::is_floating() const noexcept
	{
		return m_backend->is_floating();
	}

	bool default_window::is_focus_on_show() const noexcept
	{
		return m_backend->is_focus_on_show();
	}

	bool default_window::is_focused() const noexcept
	{
		return m_backend->is_focused();
	}

	bool default_window::is_hovered() const noexcept
	{
		return m_backend->is_hovered();
	}

	bool default_window::is_iconified() const noexcept
	{
		return m_backend->is_iconified();
	}

	bool default_window::is_maximized() const noexcept
	{
		return m_backend->is_maximized();
	}

	bool default_window::is_open() const noexcept
	{
		return m_backend->is_open();
	}

	bool default_window::is_resizable() const noexcept
	{
		return m_backend->is_resizable();
	}

	bool default_window::is_transparent() const noexcept
	{
		return m_backend->is_transparent();
	}

	bool default_window::is_visible() const noexcept
	{
		return m_backend->is_visible();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void default_window::set_auto_iconify(bool value) noexcept
	{
		m_backend->set_auto_iconify(value);
	}

	void default_window::set_clipboard(cstring value) noexcept
	{
		m_backend->set_clipboard(value);
	}

	void default_window::set_cursor(cursor_handle value) noexcept
	{
		m_backend->set_cursor(value);
	}
	
	void default_window::set_cursor_mode(int32_t value) noexcept
	{
		m_backend->set_cursor_mode(value);
	}

	void default_window::set_cursor_pos(vec2d const & value) noexcept
	{
		m_backend->set_cursor_pos(value);
	}

	void default_window::set_decorated(bool value) noexcept
	{
		m_backend->set_decorated(value);
	}

	void default_window::set_floating(bool value) noexcept
	{
		m_backend->set_floating(value);
	}

	void default_window::set_focus_on_show(bool value) noexcept
	{
		m_backend->set_focus_on_show(value);
	}

	void default_window::set_icons(size_t w, size_t h, size_t n, byte_t const * p) noexcept
	{
		m_backend->set_icons(w, h, n, p);
	}

	void default_window::set_input_mode(int32_t mode, int32_t value) noexcept
	{
		m_backend->set_input_mode(mode, value);
	}

	void default_window::set_opacity(float_t value) noexcept
	{
		m_backend->set_opacity(value);
	}

	void default_window::set_position(vec2i const & value) noexcept
	{
		m_backend->set_position(value);
	}

	void default_window::set_monitor(monitor_handle value, int_rect const & bounds) noexcept
	{
		m_backend->set_monitor(value, bounds);
	}

	void default_window::set_resizable(bool value) noexcept
	{
		m_backend->set_resizable(value);
	}

	void default_window::set_should_close(bool value) noexcept
	{
		m_backend->set_should_close(value);
	}

	void default_window::set_size(vec2i const & value) noexcept
	{
		m_backend->set_size(value);
	}

	void default_window::set_title(ds::string const & value) noexcept
	{
		m_backend->set_title(value);
	}
	
	void * default_window::set_user_pointer(void * value) noexcept
	{
		return m_backend->set_user_pointer(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t default_window::extension_supported(cstring value) noexcept
	{
		return native_window::extension_supported(value);
	}

	window_handle default_window::get_active_window() noexcept
	{
		return native_window::get_active_window();
	}

	ds::list<monitor_handle> const & default_window::get_monitors() noexcept
	{
		return native_window::get_monitors();
	}

	void * default_window::get_proc_address(cstring value) noexcept
	{
		return native_window::get_proc_address(value);
	}

	monitor_handle default_window::get_primary_monitor() noexcept
	{
		return native_window::get_primary_monitor();
	}

	duration default_window::get_time() noexcept
	{
		return native_window::get_time();
	}

	void default_window::set_active_window(window_handle value) noexcept
	{
		native_window::set_active_window(value);
	}

	window_error_callback default_window::set_error_callback(window_error_callback fn) noexcept
	{
		return native_window::set_error_callback(fn);
	}

	void default_window::set_swap_interval(int32_t value) noexcept
	{
		native_window::set_swap_interval(value);
	}

	void default_window::poll_events() noexcept
	{
		native_window::poll_events();
	}

	void default_window::swap_buffers(window_handle value) noexcept
	{
		native_window::swap_buffers(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	cursor_handle default_window::create_custom_cursor(size_t w, size_t h, byte_t const * p) noexcept
	{
		return native_window::create_custom_cursor(w, h, p);
	}

	cursor_handle default_window::create_standard_cursor(int32_t value) noexcept
	{
		return native_window::create_standard_cursor(value);
	}

	void default_window::destroy_cursor(cursor_handle value) noexcept
	{
		native_window::destroy_cursor(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_char_callback default_window::set_char_callback(window_char_callback fn) noexcept
	{
		return m_backend->set_char_callback(fn);
	}

	window_char_mods_callback default_window::set_char_mods_callback(window_char_mods_callback fn) noexcept
	{
		return m_backend->set_char_mods_callback(fn);
	}

	window_close_callback default_window::set_close_callback(window_close_callback fn) noexcept
	{
		return m_backend->set_close_callback(fn);
	}

	window_content_scale_callback default_window::set_content_scale_callback(window_content_scale_callback fn) noexcept
	{
		return m_backend->set_content_scale_callback(fn);
	}
	
	window_cursor_enter_callback default_window::set_cursor_enter_callback(window_cursor_enter_callback fn) noexcept
	{
		return m_backend->set_cursor_enter_callback(fn);
	}

	window_cursor_pos_callback default_window::set_cursor_pos_callback(window_cursor_pos_callback fn) noexcept
	{
		return m_backend->set_cursor_pos_callback(fn);
	}

	window_drop_callback default_window::set_drop_callback(window_drop_callback fn) noexcept
	{
		return m_backend->set_drop_callback(fn);
	}

	window_focus_callback default_window::set_focus_callback(window_focus_callback fn) noexcept
	{
		return m_backend->set_focus_callback(fn);
	}

	window_framebuffer_resize_callback default_window::set_framebuffer_resize_callback(window_framebuffer_resize_callback fn) noexcept
	{
		return m_backend->set_framebuffer_resize_callback(fn);
	}

	window_iconify_callback default_window::set_iconify_callback(window_iconify_callback fn) noexcept
	{
		return m_backend->set_iconify_callback(fn);
	}
	
	window_key_callback default_window::set_key_callback(window_key_callback fn) noexcept
	{
		return m_backend->set_key_callback(fn);
	}

	window_maximize_callback default_window::set_maximize_callback(window_maximize_callback fn) noexcept
	{
		return m_backend->set_maximize_callback(fn);
	}
	
	window_mouse_callback default_window::set_mouse_callback(window_mouse_callback fn) noexcept
	{
		return m_backend->set_mouse_callback(fn);
	}
	
	window_position_callback default_window::set_position_callback(window_position_callback fn) noexcept
	{
		return m_backend->set_position_callback(fn);
	}

	window_refresh_callback default_window::set_refresh_callback(window_refresh_callback fn) noexcept
	{
		return m_backend->set_refresh_callback(fn);
	}

	window_resize_callback default_window::set_resize_callback(window_resize_callback fn) noexcept
	{
		return m_backend->set_resize_callback(fn);
	}

	window_scroll_callback default_window::set_scroll_callback(window_scroll_callback fn) noexcept
	{
		return m_backend->set_scroll_callback(fn);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}