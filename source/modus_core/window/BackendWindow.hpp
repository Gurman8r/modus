#ifndef _ML_BACKEND_WINDOW_HPP_
#define _ML_BACKEND_WINDOW_HPP_

#include <modus_core/window/WindowContext.hpp>

namespace ml
{
	// base window type
	struct ML_CORE_API backend_window : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~backend_window() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		virtual bool open(
			ds::string			const &	title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			void *						userptr	= nullptr) = 0;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void focus() = 0;

		virtual void hide() = 0;

		virtual void iconify() = 0;

		virtual void maximize() = 0;

		virtual void restore() = 0;

		virtual void request_attention() = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual int_rect get_bounds() const = 0;

		virtual window_callbacks const & get_callbacks() const = 0;

		virtual cstring get_clipboard() const = 0;

		virtual vec2 get_content_scale() const = 0;

		virtual int32_t get_cursor_mode() const = 0;

		virtual vec2 get_cursor_pos() const = 0;

		virtual vec2i get_framebuffer_size() const = 0;

		virtual window_handle get_handle() const = 0;

		virtual window_hints_ get_hints() const = 0;

		ML_NODISCARD inline bool has_hints(int32_t value) const noexcept
		{
			return ML_flag_read((int32_t)get_hints(), value);
		}

		virtual int32_t get_input_mode(int32_t) const = 0;

		virtual int32_t get_key(int32_t) const = 0;

		virtual int32_t get_mouse_button(int32_t) const = 0;

		virtual window_handle get_native_handle() const = 0;

		virtual float_t get_opacity() const = 0;

		virtual vec2i get_position() const = 0;

		virtual vec2i get_size() const = 0;

		virtual ds::string const & get_title() const = 0;

		virtual void * get_user_pointer() const = 0;

		virtual window_context const * get_window_context() const = 0;

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

		virtual void set_cursor_pos(vec2d const &) = 0;

		virtual void set_decorated(bool) = 0;

		virtual void set_floating(bool) = 0;

		virtual void set_focus_on_show(bool) = 0;

		virtual void set_icons(size_t, size_t, byte_t const *, size_t) = 0;

		virtual void set_input_mode(int32_t, int32_t) = 0;

		virtual void set_opacity(float_t) = 0;

		virtual void set_position(vec2i const &) = 0;

		virtual void set_monitor(monitor_handle, int_rect const &) = 0;

		virtual void set_resizable(bool) = 0;

		virtual void set_should_close(bool) = 0;

		virtual void set_size(vec2i const &) = 0;

		virtual void set_title(ds::string const &) = 0;

		virtual void * set_user_pointer(void *) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual window_char_callback				get_char_callback() const = 0;
		virtual window_char_mods_callback			get_char_mods_callback() const = 0;
		virtual window_close_callback				get_close_callback() const = 0;
		virtual window_content_scale_callback		get_content_scale_callback() const = 0;
		virtual window_cursor_enter_callback		get_cursor_enter_callback() const = 0;
		virtual window_cursor_pos_callback			get_cursor_pos_callback() const = 0;
		virtual window_drop_callback				get_drop_callback() const = 0;
		virtual window_focus_callback				get_focus_callback() const = 0;
		virtual window_framebuffer_resize_callback	get_framebuffer_resize_callback() const = 0;
		virtual window_iconify_callback				get_iconify_callback() const = 0;
		virtual window_key_callback					get_key_callback() const = 0;
		virtual window_maximize_callback			get_maximize_callback() const = 0;
		virtual window_mouse_callback				get_mouse_callback() const = 0;
		virtual window_position_callback			get_position_callback() const = 0;
		virtual window_refresh_callback				get_refresh_callback() const = 0;
		virtual window_resize_callback				get_resize_callback() const = 0;
		virtual window_scroll_callback				get_scroll_callback() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual window_char_callback				set_char_callback				(window_char_callback) = 0;
		virtual window_char_mods_callback			set_char_mods_callback			(window_char_mods_callback) = 0;
		virtual window_close_callback				set_close_callback				(window_close_callback) = 0;
		virtual window_content_scale_callback		set_content_scale_callback		(window_content_scale_callback) = 0;
		virtual window_cursor_enter_callback		set_cursor_enter_callback		(window_cursor_enter_callback) = 0;
		virtual window_cursor_pos_callback			set_cursor_pos_callback			(window_cursor_pos_callback) = 0;
		virtual window_drop_callback				set_drop_callback				(window_drop_callback) = 0;
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

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void clear_callbacks()
		{
			set_char_callback				(nullptr);
			set_char_mods_callback			(nullptr);
			set_close_callback				(nullptr);
			set_content_scale_callback		(nullptr);
			set_cursor_enter_callback		(nullptr);
			set_cursor_pos_callback			(nullptr);
			set_drop_callback				(nullptr);
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

#endif // !_ML_BACKEND_WINDOW_HPP_