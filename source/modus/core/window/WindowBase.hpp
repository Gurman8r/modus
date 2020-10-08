#ifndef _ML_WINDOW_BASE_HPP_
#define _ML_WINDOW_BASE_HPP_

#include <core/window/WindowAPI.hpp>

namespace ml
{
	// window implementation base
	struct ML_CORE_API window_base : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~window_base() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual bool open(window_settings const &) = 0;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void iconify() = 0;

		virtual void maximize() = 0;

		virtual void restore() = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual allocator_type get_allocator() const = 0;

		virtual int_rect get_bounds() const = 0;

		virtual window_callbacks const & get_callbacks() const = 0;

		virtual cstring get_clipboard() const = 0;

		virtual vec2 get_content_scale() const = 0;

		virtual int32_t get_cursor_mode() const = 0;

		virtual vec2 get_cursor_position() const = 0;

		virtual vec2i get_framebuffer_size() const = 0;

		virtual window_handle get_handle() const = 0;

		virtual int32_t get_hints() const = 0;

		ML_NODISCARD bool has_hints(int32_t value) const noexcept
		{
			return (value & get_hints()) == value;
		}

		virtual int32_t get_input_mode(int32_t) const = 0;

		virtual int32_t get_key(int32_t) const = 0;

		virtual int32_t get_mouse_button(int32_t) const = 0;

		virtual window_handle get_native_handle() const = 0;

		virtual float_t get_opacity() const = 0;

		virtual vec2i get_position() const = 0;

		virtual vec2i get_size() const = 0;

		virtual pmr::string const & get_title() const = 0;

		virtual void * get_user_pointer() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual bool is_auto_iconify() const = 0;

		virtual bool is_decorated() const = 0;

		virtual bool is_floating() const = 0;

		virtual bool is_focused() const = 0;

		virtual bool is_focus_on_show() const = 0;

		virtual bool is_hovered() const = 0;

		virtual bool is_iconified() const = 0;

		virtual bool is_maximized() const = 0;

		virtual bool is_open() const = 0;

		virtual bool is_resizable() const = 0;

		virtual bool is_transparent() const = 0;

		virtual bool is_visible() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void set_auto_iconify(bool) = 0;

		virtual void set_clipboard(cstring) = 0;

		virtual void set_cursor(cursor_handle) = 0;

		virtual void set_cursor_mode(int32_t) = 0;

		virtual void set_cursor_position(vec2d const &) = 0;

		virtual void set_decorated(bool) = 0;

		virtual void set_floating(bool) = 0;

		virtual void set_focus_on_show(bool) = 0;

		virtual void set_icons(size_t, size_t, size_t, byte_t const *) = 0;

		virtual void set_input_mode(int32_t, int32_t) = 0;

		virtual void set_opacity(float_t) = 0;

		virtual void set_position(vec2i const &) = 0;

		virtual void set_monitor(monitor_handle, int_rect const &) = 0;

		virtual void set_resizable(bool) = 0;

		virtual void set_should_close(bool) = 0;

		virtual void set_size(vec2i const &) = 0;

		virtual void set_title(pmr::string const &) = 0;

		virtual void set_user_pointer(void *) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static window_handle get_context_current() = delete;

		static void * get_proc_address(cstring) = delete;
		
		static pmr::vector<monitor_handle> const & get_monitors() = delete;

		static monitor_handle get_primary_monitor() = delete;

		static duration get_time() = delete;

		static int32_t is_extension_supported(cstring) = delete;

		static void make_context_current(window_handle) = delete;

		static void poll_events() = delete;

		static void swap_buffers(window_handle) = delete;

		static void swap_interval(int32_t) = delete;

		static cursor_handle create_custom_cursor(size_t, size_t, byte_t const *) = delete;

		static cursor_handle create_standard_cursor(int32_t) = delete;

		static void destroy_cursor(cursor_handle) = delete;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual window_char_callback				set_char_callback				(window_char_callback) = 0;
		virtual window_char_mods_callback			set_char_mods_callback			(window_char_mods_callback) = 0;
		virtual window_close_callback				set_close_callback				(window_close_callback) = 0;
		virtual window_content_scale_callback		set_content_scale_callback		(window_content_scale_callback) = 0;
		virtual window_cursor_enter_callback		set_cursor_enter_callback		(window_cursor_enter_callback) = 0;
		virtual window_cursor_position_callback		set_cursor_position_callback	(window_cursor_position_callback) = 0;
		virtual window_drop_callback				set_drop_callback				(window_drop_callback) = 0;
		virtual window_error_callback				set_error_callback				(window_error_callback) = 0;
		virtual window_focus_callback				set_focus_callback				(window_focus_callback) = 0;
		virtual window_framebuffer_resize_callback	set_framebuffer_resize_callback	(window_framebuffer_resize_callback) = 0;
		virtual window_iconify_callback				set_iconify_callback			(window_iconify_callback) = 0;
		virtual window_key_callback					set_key_callback				(window_key_callback) = 0;
		virtual window_maximize_callback			set_maximize_callback			(window_maximize_callback) = 0;
		virtual window_mouse_callback				set_mouse_callback				(window_mouse_callback) = 0;
		virtual window_position_callback			set_position_callback			(window_position_callback) = 0;
		virtual window_refresh_callback				set_refresh_callback			(window_refresh_callback) = 0;
		virtual window_resize_callback				set_resize_callback				(window_resize_callback) = 0;
		virtual window_scroll_callback				set_scroll_callback				(window_scroll_callback) = 0;

		void clear_callbacks() noexcept
		{
			set_char_callback				(nullptr);
			set_char_mods_callback			(nullptr);
			set_close_callback				(nullptr);
			set_content_scale_callback		(nullptr);
			set_cursor_enter_callback		(nullptr);
			set_cursor_position_callback	(nullptr);
			set_drop_callback				(nullptr);
			set_error_callback				(nullptr);
			set_focus_callback				(nullptr);
			set_framebuffer_resize_callback	(nullptr);
			set_iconify_callback			(nullptr);
			set_key_callback				(nullptr);
			set_maximize_callback			(nullptr);
			set_mouse_callback				(nullptr);
			set_position_callback			(nullptr);
			set_refresh_callback			(nullptr);
			set_resize_callback				(nullptr);
			set_scroll_callback				(nullptr);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_WINDOW_BASE_HPP_