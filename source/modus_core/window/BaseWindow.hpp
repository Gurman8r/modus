#ifndef _ML_BASE_WINDOW_HPP_
#define _ML_BASE_WINDOW_HPP_

#include <modus_core/window/WindowAPI.hpp>

// BACKEND
namespace ml
{
	// backend window
	struct ML_CORE_API base_window : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		virtual ~base_window() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			string const &			title,
			video_mode const &			vm		= {},
			context_settings const &	cs		= {},
			window_hints_				hints	= window_hints_default
		) = 0;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void focus() = 0;

		virtual void hide() = 0;

		virtual void iconify() = 0;

		virtual void maximize() = 0;

		virtual void restore() = 0;

		virtual void request_attention() = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD inline int_rect get_bounds() const noexcept
		{
			return int_rect{ get_position(), get_size() };
		}

		ML_NODISCARD virtual window_callbacks const & get_callbacks() const = 0;

		ML_NODISCARD virtual string get_clipboard() const = 0;

		ML_NODISCARD virtual vec2 get_content_scale() const = 0;

		ML_NODISCARD virtual vec2i get_framebuffer_size() const = 0;

		ML_NODISCARD virtual window_handle get_handle() const = 0;

		ML_NODISCARD virtual window_hints_ get_hints() const = 0;

		ML_NODISCARD inline bool has_hints(int32 value) const noexcept
		{
			return ML_flag_read((int32)get_hints(), value);
		}

		ML_NODISCARD virtual int32 get_input_mode(int32) const = 0;

		ML_NODISCARD virtual int32 get_key(int32) const = 0;

		ML_NODISCARD virtual int32 get_mouse_button(int32) const = 0;

		ML_NODISCARD virtual vec2 get_mouse_pos() const = 0;

		ML_NODISCARD virtual window_handle get_native_handle() const = 0;

		ML_NODISCARD virtual float32 get_opacity() const = 0;

		ML_NODISCARD virtual vec2i get_position() const = 0;

		ML_NODISCARD virtual vec2i get_size() const = 0;

		ML_NODISCARD virtual string const & get_title() const = 0;

		ML_NODISCARD virtual void * get_user_pointer() const = 0;

		ML_NODISCARD virtual int_rect get_window_frame_size() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool is_auto_iconify() const = 0;

		ML_NODISCARD virtual bool is_decorated() const = 0;

		ML_NODISCARD virtual bool is_floating() const = 0;

		ML_NODISCARD virtual bool is_focused() const = 0;

		ML_NODISCARD virtual bool is_focus_on_show() const = 0;

		ML_NODISCARD virtual bool is_hovered() const = 0;

		ML_NODISCARD virtual bool is_iconified() const = 0;

		ML_NODISCARD virtual bool is_maximized() const = 0;

		ML_NODISCARD virtual bool is_open() const = 0;

		ML_NODISCARD virtual bool is_resizable() const = 0;

		ML_NODISCARD virtual bool is_transparent() const = 0;

		ML_NODISCARD virtual bool is_visible() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void set_auto_iconify(bool) = 0;

		virtual void set_clipboard(string const &) = 0;

		virtual void set_cursor(cursor_handle) = 0;

		virtual void set_cursor_mode(int32) = 0;

		virtual void set_mouse_pos(vec2d const &) = 0;

		virtual void set_decorated(bool) = 0;

		virtual void set_floating(bool) = 0;

		virtual void set_focus_on_show(bool) = 0;

		virtual void set_icons(int32, size_t, size_t, byte const *) = 0;

		virtual void set_input_mode(int32, int32) = 0;

		virtual void set_opacity(float32) = 0;

		virtual void set_position(vec2i const &) = 0;

		virtual void set_monitor(monitor_handle, int_rect const &) = 0;

		virtual void set_resizable(bool) = 0;

		virtual void set_should_close(bool) = 0;

		virtual void set_size(vec2i const &) = 0;

		virtual void set_title(string const &) = 0;

		virtual void set_user_pointer(void *) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual window_char_callback get_char_callback() const = 0;
		
		ML_NODISCARD virtual window_char_mods_callback get_char_mods_callback() const = 0;
		
		ML_NODISCARD virtual window_close_callback get_close_callback() const = 0;
		
		ML_NODISCARD virtual window_content_scale_callback get_content_scale_callback() const = 0;
		
		ML_NODISCARD virtual window_mouse_enter_callback get_mouse_enter_callback() const = 0;
		
		ML_NODISCARD virtual window_mouse_pos_callback get_mouse_pos_callback() const = 0;
		
		ML_NODISCARD virtual window_drop_callback get_drop_callback() const = 0;
		
		ML_NODISCARD virtual window_focus_callback get_focus_callback() const = 0;
		
		ML_NODISCARD virtual window_framebuffer_resize_callback get_framebuffer_resize_callback() const = 0;
		
		ML_NODISCARD virtual window_iconify_callback get_iconify_callback() const = 0;
		
		ML_NODISCARD virtual window_key_callback get_key_callback() const = 0;
		
		ML_NODISCARD virtual window_maximize_callback get_maximize_callback() const = 0;
		
		ML_NODISCARD virtual window_mouse_button_callback get_mouse_button_callback() const = 0;
		
		ML_NODISCARD virtual window_position_callback get_position_callback() const = 0;
		
		ML_NODISCARD virtual window_refresh_callback get_refresh_callback() const = 0;
		
		ML_NODISCARD virtual window_resize_callback get_resize_callback() const = 0;
		
		ML_NODISCARD virtual window_scroll_callback get_scroll_callback() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual window_char_callback set_char_callback(window_char_callback) = 0;
		
		virtual window_char_mods_callback set_char_mods_callback(window_char_mods_callback) = 0;
		
		virtual window_close_callback set_close_callback(window_close_callback) = 0;
		
		virtual window_content_scale_callback set_content_scale_callback(window_content_scale_callback) = 0;
		
		virtual window_mouse_enter_callback set_mouse_enter_callback(window_mouse_enter_callback) = 0;
		
		virtual window_mouse_pos_callback set_mouse_pos_callback(window_mouse_pos_callback) = 0;
		
		virtual window_drop_callback set_drop_callback(window_drop_callback) = 0;
		
		virtual window_focus_callback set_focus_callback(window_focus_callback) = 0;
		
		virtual window_framebuffer_resize_callback set_framebuffer_resize_callback(window_framebuffer_resize_callback) = 0;
		
		virtual window_iconify_callback set_iconify_callback(window_iconify_callback) = 0;
		
		virtual window_key_callback set_key_callback(window_key_callback) = 0;
		
		virtual window_maximize_callback set_maximize_callback(window_maximize_callback) = 0;
		
		virtual window_mouse_button_callback set_mouse_button_callback(window_mouse_button_callback) = 0;
		
		virtual window_position_callback set_position_callback(window_position_callback) = 0;
		
		virtual window_refresh_callback set_refresh_callback(window_refresh_callback) = 0;
		
		virtual window_resize_callback set_resize_callback(window_resize_callback) = 0;
		
		virtual window_scroll_callback set_scroll_callback(window_scroll_callback) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		inline void clear_callbacks() noexcept
		{
			set_char_callback				(nullptr);
			set_char_mods_callback			(nullptr);
			set_key_callback				(nullptr);
			set_mouse_enter_callback		(nullptr);
			set_mouse_pos_callback			(nullptr);
			set_mouse_button_callback		(nullptr);
			set_scroll_callback				(nullptr);

			set_close_callback				(nullptr);
			set_content_scale_callback		(nullptr);
			set_drop_callback				(nullptr);
			set_focus_callback				(nullptr);
			set_framebuffer_resize_callback	(nullptr);
			set_iconify_callback			(nullptr);
			set_maximize_callback			(nullptr);
			set_position_callback			(nullptr);
			set_refresh_callback			(nullptr);
			set_resize_callback				(nullptr);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_BASE_WINDOW_HPP_