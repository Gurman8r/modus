#include <modus_core/window/NativeWindow.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef ML_IMPL_WINDOW_GLFW
#include "../platform/glfw/GLFW_Window.hpp"
using impl_window = _ML glfw_window;
#endif

// etc...

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	native_window::native_window(allocator_type alloc) noexcept
		: m_impl{ new impl_window{ alloc } }
	{
		ML_assert_msg(m_impl, "failed creating native_window implementation");
	}

	native_window::native_window(
		pmr::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints,
		allocator_type				alloc
	) noexcept : native_window{ alloc }
	{
		ML_assert(open(title, vm, cs, hints));
	}

	native_window::~native_window() noexcept
	{
		clear_callbacks();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool native_window::open(
		pmr::string			const & title,
		video_mode			const & vm,
		context_settings	const & cs,
		window_hints_				hints
	)
	{
		// check already open
		if (is_open()) { return debug::error("native_window is already open"); }

		// open native_window
		if (!m_impl->open(title, vm, cs, hints)) { return debug::error("failed opening native_window"); }
		
		// make current context
		make_context_current(get_handle());

		// user pointer
		set_user_pointer(this);

		// centered
		set_position((video_mode::desktop_mode().resolution - vm.resolution) / 2);

		// maximized
		if (has_hints(window_hints_maximized)) { maximize(); }

		// success
		return is_open();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void native_window::focus() noexcept
	{
		m_impl->focus();
	}

	void native_window::hide() noexcept
	{
		m_impl->hide();
	}

	void native_window::iconify() noexcept
	{
		m_impl->iconify();
	}

	void native_window::maximize() noexcept
	{
		m_impl->maximize();
	}

	void native_window::restore() noexcept
	{
		m_impl->restore();
	}

	void native_window::request_attention() noexcept
	{
		m_impl->request_attention();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	native_window::allocator_type native_window::get_allocator() const noexcept
	{
		return m_impl->get_allocator();
	}

	int_rect native_window::get_bounds() const noexcept
	{
		return m_impl->get_bounds();
	}

	window_callbacks const & native_window::get_callbacks() const noexcept
	{
		return m_impl->get_callbacks();
	}

	cstring native_window::get_clipboard() const noexcept
	{
		return m_impl->get_clipboard();
	}

	vec2 native_window::get_content_scale() const noexcept
	{
		return m_impl->get_content_scale();
	}

	vec2 native_window::get_cursor_pos() const noexcept
	{
		return m_impl->get_cursor_pos();
	}

	int32_t native_window::get_cursor_mode() const noexcept
	{
		return m_impl->get_cursor_mode();
	}

	vec2i native_window::get_framebuffer_size() const noexcept
	{
		return m_impl->get_framebuffer_size();
	}

	window_handle native_window::get_handle() const noexcept
	{
		return m_impl->get_handle();
	}

	window_hints_ native_window::get_hints() const noexcept
	{
		return m_impl->get_hints();
	}

	int32_t native_window::get_input_mode(int32_t value) const noexcept
	{
		return m_impl->get_input_mode(value);
	}

	int32_t	native_window::get_key(int32_t value) const noexcept
	{
		return m_impl->get_key(value);
	}

	int32_t	native_window::get_mouse_button(int32_t value) const noexcept
	{
		return m_impl->get_mouse_button(value);
	}

	window_handle native_window::get_native_handle() const noexcept
	{
		return m_impl->get_native_handle();
	}

	float_t native_window::get_opacity() const noexcept
	{
		return m_impl->get_opacity();
	}

	vec2i native_window::get_position() const noexcept
	{
		return m_impl->get_position();
	}

	vec2i native_window::get_size() const noexcept
	{
		return m_impl->get_size();
	}

	pmr::string const & native_window::get_title() const noexcept
	{
		return m_impl->get_title();
	}

	void * native_window::get_user_pointer() const noexcept
	{
		return m_impl->get_user_pointer();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool native_window::is_auto_iconify() const noexcept
	{
		return m_impl->is_auto_iconify();
	}

	bool native_window::is_decorated() const noexcept
	{
		return m_impl->is_decorated();
	}

	bool native_window::is_floating() const noexcept
	{
		return m_impl->is_floating();
	}

	bool native_window::is_focus_on_show() const noexcept
	{
		return m_impl->is_focus_on_show();
	}

	bool native_window::is_focused() const noexcept
	{
		return m_impl->is_focused();
	}

	bool native_window::is_hovered() const noexcept
	{
		return m_impl->is_hovered();
	}

	bool native_window::is_iconified() const noexcept
	{
		return m_impl->is_iconified();
	}

	bool native_window::is_maximized() const noexcept
	{
		return m_impl->is_maximized();
	}

	bool native_window::is_open() const noexcept
	{
		return m_impl->is_open();
	}

	bool native_window::is_resizable() const noexcept
	{
		return m_impl->is_resizable();
	}

	bool native_window::is_transparent() const noexcept
	{
		return m_impl->is_transparent();
	}

	bool native_window::is_visible() const noexcept
	{
		return m_impl->is_visible();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void native_window::set_auto_iconify(bool value) noexcept
	{
		m_impl->set_auto_iconify(value);
	}

	void native_window::set_clipboard(cstring value) noexcept
	{
		m_impl->set_clipboard(value);
	}

	void native_window::set_cursor(cursor_handle value) noexcept
	{
		m_impl->set_cursor(value);
	}
	
	void native_window::set_cursor_mode(int32_t value) noexcept
	{
		m_impl->set_cursor_mode(value);
	}

	void native_window::set_cursor_pos(vec2d const & value) noexcept
	{
		m_impl->set_cursor_pos(value);
	}

	void native_window::set_decorated(bool value) noexcept
	{
		m_impl->set_decorated(value);
	}

	void native_window::set_floating(bool value) noexcept
	{
		m_impl->set_floating(value);
	}

	void native_window::set_focus_on_show(bool value) noexcept
	{
		m_impl->set_focus_on_show(value);
	}

	void native_window::set_icons(size_t w, size_t h, size_t n, byte_t const * p) noexcept
	{
		m_impl->set_icons(w, h, n, p);
	}

	void native_window::set_input_mode(int32_t mode, int32_t value) noexcept
	{
		m_impl->set_input_mode(mode, value);
	}

	void native_window::set_opacity(float_t value) noexcept
	{
		m_impl->set_opacity(value);
	}

	void native_window::set_position(vec2i const & value) noexcept
	{
		m_impl->set_position(value);
	}

	void native_window::set_monitor(monitor_handle value, int_rect const & bounds) noexcept
	{
		m_impl->set_monitor(value, bounds);
	}

	void native_window::set_resizable(bool value) noexcept
	{
		m_impl->set_resizable(value);
	}

	void native_window::set_should_close(bool value) noexcept
	{
		m_impl->set_should_close(value);
	}

	void native_window::set_size(vec2i const & value) noexcept
	{
		m_impl->set_size(value);
	}

	void native_window::set_title(pmr::string const & value) noexcept
	{
		m_impl->set_title(value);
	}

	void native_window::set_user_pointer(void * value) noexcept
	{
		m_impl->set_user_pointer(value);
	}
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_handle native_window::get_context_current() noexcept
	{
		return impl_window::get_context_current();
	}

	void * native_window::get_proc_address(cstring value) noexcept
	{
		return impl_window::get_proc_address(value);
	}

	pmr::vector<monitor_handle> const & native_window::get_monitors() noexcept
	{
		return impl_window::get_monitors();
	}

	monitor_handle native_window::get_primary_monitor() noexcept
	{
		return impl_window::get_primary_monitor();
	}

	duration native_window::get_time() noexcept
	{
		return impl_window::get_time();
	}

	int32_t native_window::extension_supported(cstring value) noexcept
	{
		return impl_window::extension_supported(value);
	}

	void native_window::make_context_current(window_handle value) noexcept
	{
		impl_window::make_context_current(value);
	}

	void native_window::poll_events() noexcept
	{
		impl_window::poll_events();
	}

	void native_window::swap_buffers(window_handle value) noexcept
	{
		impl_window::swap_buffers(value);
	}

	void native_window::swap_interval(int32_t value) noexcept
	{
		impl_window::swap_interval(value);
	}

	cursor_handle native_window::create_custom_cursor(size_t w, size_t h, byte_t const * p) noexcept
	{
		return impl_window::create_custom_cursor(w, h, p);
	}

	cursor_handle native_window::create_standard_cursor(int32_t value) noexcept
	{
		return impl_window::create_standard_cursor(value);
	}

	void native_window::destroy_cursor(cursor_handle value) noexcept
	{
		impl_window::destroy_cursor(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	window_char_callback native_window::set_char_callback(window_char_callback fn) noexcept
	{
		return m_impl->set_char_callback(fn);
	}

	window_char_mods_callback native_window::set_char_mods_callback(window_char_mods_callback fn) noexcept
	{
		return m_impl->set_char_mods_callback(fn);
	}

	window_close_callback native_window::set_close_callback(window_close_callback fn) noexcept
	{
		return m_impl->set_close_callback(fn);
	}

	window_content_scale_callback native_window::set_content_scale_callback(window_content_scale_callback fn) noexcept
	{
		return m_impl->set_content_scale_callback(fn);
	}
	
	window_cursor_enter_callback native_window::set_cursor_enter_callback(window_cursor_enter_callback fn) noexcept
	{
		return m_impl->set_cursor_enter_callback(fn);
	}

	window_cursor_pos_callback native_window::set_cursor_pos_callback(window_cursor_pos_callback fn) noexcept
	{
		return m_impl->set_cursor_pos_callback(fn);
	}

	window_drop_callback native_window::set_drop_callback(window_drop_callback fn) noexcept
	{
		return m_impl->set_drop_callback(fn);
	}

	window_error_callback native_window::set_error_callback(window_error_callback fn) noexcept
	{
		return m_impl->set_error_callback(fn);
	}

	window_focus_callback native_window::set_focus_callback(window_focus_callback fn) noexcept
	{
		return m_impl->set_focus_callback(fn);
	}

	window_framebuffer_resize_callback native_window::set_framebuffer_resize_callback(window_framebuffer_resize_callback fn) noexcept
	{
		return m_impl->set_framebuffer_resize_callback(fn);
	}

	window_iconify_callback native_window::set_iconify_callback(window_iconify_callback fn) noexcept
	{
		return m_impl->set_iconify_callback(fn);
	}
	
	window_key_callback native_window::set_key_callback(window_key_callback fn) noexcept
	{
		return m_impl->set_key_callback(fn);
	}

	window_maximize_callback native_window::set_maximize_callback(window_maximize_callback fn) noexcept
	{
		return m_impl->set_maximize_callback(fn);
	}
	
	window_mouse_callback native_window::set_mouse_callback(window_mouse_callback fn) noexcept
	{
		return m_impl->set_mouse_callback(fn);
	}
	
	window_position_callback native_window::set_position_callback(window_position_callback fn) noexcept
	{
		return m_impl->set_position_callback(fn);
	}

	window_refresh_callback native_window::set_refresh_callback(window_refresh_callback fn) noexcept
	{
		return m_impl->set_refresh_callback(fn);
	}

	window_resize_callback native_window::set_resize_callback(window_resize_callback fn) noexcept
	{
		return m_impl->set_resize_callback(fn);
	}

	window_scroll_callback native_window::set_scroll_callback(window_scroll_callback fn) noexcept
	{
		return m_impl->set_scroll_callback(fn);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}