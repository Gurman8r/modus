#if defined(ML_IMPL_WINDOW_GLFW)
#ifndef _ML_IMPL_WINDOW_GLFW_HPP_
#define _ML_IMPL_WINDOW_GLFW_HPP_

#include <window/WindowBase.hpp>

struct GLFWwindow;
struct GLFWmonitor;

namespace ml
{
	// glfw window implementation
	struct glfw_window final : window_base
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit glfw_window(allocator_type alloc) noexcept;

		explicit glfw_window(window_settings const & settings, allocator_type alloc) noexcept;

		~glfw_window() override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool open(window_settings const & ws) override;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void iconify() override;

		void maximize() override;

		void restore() override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		allocator_type get_allocator() const override;

		int_rect get_bounds() const override;

		window_callbacks const & get_callbacks() const override;

		cstring get_clipboard() const override;

		vec2 get_content_scale() const override;

		int32_t get_cursor_mode() const override;

		vec2 get_cursor_position() const override;

		vec2i get_framebuffer_size() const override;

		window_handle get_handle() const override;

		int32_t get_hints() const override;

		int32_t get_input_mode(int32_t mode) const override;

		int32_t get_key(int32_t key) const override;

		int32_t get_mouse_button(int32_t button) const override;

		window_handle get_native_handle() const override;

		float_t get_opacity() const override;

		vec2i get_position() const override;

		vec2i get_size() const override;

		pmr::string const & get_title() const override;

		void * get_user_pointer() const override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool is_auto_iconify() const override;

		bool is_decorated() const override;

		bool is_floating() const override;

		bool is_focus_on_show() const override;

		bool is_focused() const override;

		bool is_hovered() const override;

		bool is_iconified() const override;

		bool is_maximized() const override;

		bool is_open() const override;

		bool is_resizable() const override;

		bool is_transparent() const override;

		bool is_visible() const override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_auto_iconify(bool value) override;

		void set_clipboard(cstring value) override;
		
		void set_cursor(cursor_handle value) override;
		
		void set_cursor_mode(int32_t value) override;
		
		void set_cursor_position(vec2d const & value) override;

		void set_decorated(bool value) override;

		void set_floating(bool value) override;

		void set_focus_on_show(bool value) override;
		
		void set_icons(size_t w, size_t h, size_t n, byte_t const * p) override;

		void set_input_mode(int32_t mode, int32_t value) override;

		void set_opacity(float_t value) override;
		
		void set_position(vec2i const & value) override;
		
		void set_monitor(monitor_handle value, int_rect const & bounds = {}) override;

		void set_resizable(bool value) override;

		void set_should_close(bool value) override;

		void set_size(vec2i const & value) override;
		
		void set_title(pmr::string const & value) override;

		void set_user_pointer(void * value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static window_handle get_context_current();

		static void * get_proc_address(cstring value);
		
		static pmr::vector<monitor_handle> const & get_monitors();

		static monitor_handle get_primary_monitor();

		static duration get_time();

		static int32_t is_extension_supported(cstring value);

		static void make_context_current(window_handle value);

		static void poll_events();

		static void swap_buffers(window_handle value);

		static void swap_interval(int32_t value);

		static cursor_handle create_custom_cursor(size_t w, size_t h, byte_t const * p);

		static cursor_handle create_standard_cursor(int32_t value);

		static void destroy_cursor(cursor_handle value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		window_char_callback				set_char_callback				(window_char_callback				fn) override;
		window_char_mods_callback			set_char_mods_callback			(window_char_mods_callback			fn) override;
		window_close_callback				set_close_callback				(window_close_callback				fn) override;
		window_content_scale_callback		set_content_scale_callback		(window_content_scale_callback		fn) override;
		window_cursor_enter_callback		set_cursor_enter_callback		(window_cursor_enter_callback		fn) override;
		window_cursor_position_callback		set_cursor_position_callback	(window_cursor_position_callback	fn) override;
		window_drop_callback				set_drop_callback				(window_drop_callback				fn) override;
		window_error_callback				set_error_callback				(window_error_callback				fn) override;
		window_focus_callback				set_focus_callback				(window_focus_callback				fn) override;
		window_framebuffer_resize_callback	set_framebuffer_resize_callback	(window_framebuffer_resize_callback	fn) override;
		window_iconify_callback				set_iconify_callback			(window_iconify_callback			fn) override;
		window_key_callback					set_key_callback				(window_key_callback				fn) override;
		window_maximize_callback			set_maximize_callback			(window_maximize_callback			fn) override;
		window_mouse_callback				set_mouse_callback				(window_mouse_callback				fn) override;
		window_position_callback			set_position_callback			(window_position_callback			fn) override;
		window_refresh_callback				set_refresh_callback			(window_refresh_callback			fn) override;
		window_resize_callback				set_resize_callback				(window_resize_callback				fn) override;
		window_scroll_callback				set_scroll_callback				(window_scroll_callback				fn) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		allocator_type		m_alloc		; // 
		pmr::string			m_title		; // 
		GLFWwindow	*		m_window	; // 
		GLFWmonitor	*		m_monitor	; // 
		int32_t				m_hints		; // 
		window_callbacks	m_clbk		; //

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_IMPL_WINDOW_GLFW_HPP_
#endif // ML_IMPL_WINDOW_GLFW