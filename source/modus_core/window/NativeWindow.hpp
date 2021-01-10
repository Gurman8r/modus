#ifndef _ML_NATIVE_WINDOW_HPP_
#define _ML_NATIVE_WINDOW_HPP_

#include <modus_core/window/BaseWindow.hpp>

namespace ml
{
	// native window
	struct ML_CORE_API native_window : base_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using base_window::allocator_type;

		using base_window::base_window;

		native_window(allocator_type alloc = {}) noexcept;

		virtual ~native_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			string const &			title,
			video_mode const &			vm		= {},
			context_settings const &	cs		= {},
			window_hints_				hints	= window_hints_default
		) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void focus() noexcept final { m_impl->focus(); }

		void hide() noexcept final { m_impl->hide(); }

		void iconify() noexcept final { m_impl->iconify(); }

		void maximize() noexcept final { m_impl->maximize(); }

		void restore() noexcept final { m_impl->restore(); }

		void request_attention() noexcept final { m_impl->request_attention(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD window_callbacks const & get_callbacks() const noexcept final { return m_impl->get_callbacks(); }

		ML_NODISCARD string get_clipboard() const noexcept final { return m_impl->get_clipboard(); }

		ML_NODISCARD vec2 get_content_scale() const noexcept final { return m_impl->get_content_scale(); }

		ML_NODISCARD vec2i get_framebuffer_size() const noexcept final { return m_impl->get_framebuffer_size(); }

		ML_NODISCARD window_handle get_handle() const noexcept final { return m_impl->get_handle(); }

		ML_NODISCARD window_hints_ get_hints() const noexcept final { return m_impl->get_hints(); }

		ML_NODISCARD int32 get_input_mode(int32 value) const noexcept final { return m_impl->get_input_mode(value); }

		ML_NODISCARD int32 get_key(int32 value) const noexcept final { return m_impl->get_key(value); }

		ML_NODISCARD int32 get_mouse_button(int32 value) const noexcept final { return m_impl->get_mouse_button(value); }

		ML_NODISCARD vec2 get_mouse_pos() const noexcept final { return m_impl->get_mouse_pos(); }

		ML_NODISCARD window_handle get_native_handle() const noexcept final { return m_impl->get_native_handle(); }

		ML_NODISCARD float32 get_opacity() const noexcept final { return m_impl->get_opacity(); }

		ML_NODISCARD vec2i get_position() const noexcept final { return m_impl->get_position(); }

		ML_NODISCARD vec2i get_size() const noexcept final { return m_impl->get_size(); }

		ML_NODISCARD string const & get_title() const noexcept final { return m_impl->get_title(); }

		ML_NODISCARD void * get_user_pointer() const noexcept final { return m_impl->get_user_pointer(); }

		ML_NODISCARD int_rect get_window_frame_size() const noexcept final { return m_impl->get_window_frame_size(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool is_auto_iconify() const noexcept final { return m_impl->is_auto_iconify(); }

		ML_NODISCARD bool is_decorated() const noexcept final { return m_impl->is_decorated(); }

		ML_NODISCARD bool is_floating() const noexcept final { return m_impl->is_floating(); }

		ML_NODISCARD bool is_focus_on_show() const noexcept final { return m_impl->is_focus_on_show(); }

		ML_NODISCARD bool is_focused() const noexcept final { return m_impl->is_focused(); }

		ML_NODISCARD bool is_hovered() const noexcept final { return m_impl->is_hovered(); }

		ML_NODISCARD bool is_iconified() const noexcept final { return m_impl->is_iconified(); }

		ML_NODISCARD bool is_maximized() const noexcept final { return m_impl->is_maximized(); }

		ML_NODISCARD bool is_open() const noexcept final { return m_impl->is_open(); }

		ML_NODISCARD bool is_resizable() const noexcept final { return m_impl->is_resizable(); }

		ML_NODISCARD bool is_transparent() const noexcept final { return m_impl->is_transparent(); }

		ML_NODISCARD bool is_visible() const noexcept final { return m_impl->is_visible(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_auto_iconify(bool value) noexcept final { m_impl->set_auto_iconify(value); }

		void set_clipboard(string const & value) noexcept final { m_impl->set_clipboard(value); }
		
		void set_cursor(cursor_handle value) noexcept final { m_impl->set_cursor(value); }
		
		void set_cursor_mode(int32 value) noexcept final { m_impl->set_cursor_mode(value); }
		
		void set_mouse_pos(vec2d const & value) noexcept final { m_impl->set_mouse_pos(value); }

		void set_decorated(bool value) noexcept final { m_impl->set_decorated(value); }

		void set_floating(bool value) noexcept final { m_impl->set_floating(value); }

		void set_focus_on_show(bool value) noexcept final { m_impl->set_focus_on_show(value); }
		
		void set_icons(int32 n, size_t w, size_t h, byte const * p) noexcept final { m_impl->set_icons(n, w, h, p); }

		void set_input_mode(int32 mode, int32 value) noexcept final { m_impl->set_input_mode(mode, value); }
		
		void set_opacity(float32 value) noexcept final { m_impl->set_opacity(value); }

		void set_position(vec2i const & value) noexcept final { m_impl->set_position(value); }
		
		void set_monitor(monitor_handle value, int_rect const & bounds = {}) noexcept final { m_impl->set_monitor(value, bounds); }

		void set_resizable(bool value) noexcept final { m_impl->set_resizable(value); }

		void set_should_close(bool value) noexcept final { m_impl->set_should_close(value); }

		void set_size(vec2i const & value) noexcept final { m_impl->set_size(value); }
		
		void set_title(string const & value) noexcept final { m_impl->set_title(value); }

		void set_user_pointer(void * value) noexcept final { m_impl->set_user_pointer(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD window_char_callback get_char_callback() const noexcept final { return m_impl->get_char_callback(); }
		
		ML_NODISCARD window_char_mods_callback get_char_mods_callback() const noexcept final { return m_impl->get_char_mods_callback(); }
		
		ML_NODISCARD window_close_callback get_close_callback() const noexcept final { return m_impl->get_close_callback(); }
		
		ML_NODISCARD window_content_scale_callback get_content_scale_callback() const noexcept final { return m_impl->get_content_scale_callback(); }
		
		ML_NODISCARD window_drop_callback get_drop_callback() const noexcept final { return m_impl->get_drop_callback(); }
		
		ML_NODISCARD window_focus_callback get_focus_callback() const noexcept final { return m_impl->get_focus_callback(); }
		
		ML_NODISCARD window_framebuffer_resize_callback get_framebuffer_resize_callback() const noexcept final { return m_impl->get_framebuffer_resize_callback(); }
		
		ML_NODISCARD window_iconify_callback get_iconify_callback() const noexcept final { return m_impl->get_iconify_callback(); }
		
		ML_NODISCARD window_key_callback get_key_callback() const noexcept final { return m_impl->get_key_callback(); }
		
		ML_NODISCARD window_maximize_callback get_maximize_callback() const noexcept final { return m_impl->get_maximize_callback(); }
		
		ML_NODISCARD window_mouse_button_callback get_mouse_button_callback() const noexcept final { return m_impl->get_mouse_button_callback(); }

		ML_NODISCARD window_mouse_enter_callback get_mouse_enter_callback() const noexcept final { return m_impl->get_mouse_enter_callback(); }

		ML_NODISCARD window_mouse_pos_callback get_mouse_pos_callback() const noexcept final { return m_impl->get_mouse_pos_callback(); }
		
		ML_NODISCARD window_position_callback get_position_callback() const noexcept final { return m_impl->get_position_callback(); }
		
		ML_NODISCARD window_refresh_callback get_refresh_callback() const noexcept final { return m_impl->get_refresh_callback(); }
		
		ML_NODISCARD window_resize_callback get_resize_callback() const noexcept final { return m_impl->get_resize_callback(); }
		
		ML_NODISCARD window_scroll_callback get_scroll_callback() const noexcept final { return m_impl->get_scroll_callback(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		window_char_callback set_char_callback(window_char_callback value) noexcept final { return m_impl->set_char_callback(value); }
		
		window_char_mods_callback set_char_mods_callback(window_char_mods_callback value) noexcept final { return m_impl->set_char_mods_callback(value); }
		
		window_close_callback set_close_callback(window_close_callback value) noexcept final { return m_impl->set_close_callback(value); }
		
		window_content_scale_callback set_content_scale_callback(window_content_scale_callback value) noexcept final { return m_impl->set_content_scale_callback(value); }
		
		window_drop_callback set_drop_callback(window_drop_callback value) noexcept final { return m_impl->set_drop_callback(value); }
		
		window_focus_callback set_focus_callback(window_focus_callback value) noexcept final { return m_impl->set_focus_callback(value); }
		
		window_framebuffer_resize_callback set_framebuffer_resize_callback(window_framebuffer_resize_callback value) noexcept final { return m_impl->set_framebuffer_resize_callback(value); }
		
		window_iconify_callback set_iconify_callback(window_iconify_callback value) noexcept final { return m_impl->set_iconify_callback(value); }
		
		window_key_callback set_key_callback(window_key_callback value) noexcept final { return m_impl->set_key_callback(value); }
		
		window_maximize_callback set_maximize_callback(window_maximize_callback value) noexcept final { return m_impl->set_maximize_callback(value); }
		
		window_mouse_button_callback set_mouse_button_callback(window_mouse_button_callback value) noexcept final { return m_impl->set_mouse_button_callback(value); }

		window_mouse_enter_callback set_mouse_enter_callback(window_mouse_enter_callback value) noexcept final { return m_impl->set_mouse_enter_callback(value); }

		window_mouse_pos_callback set_mouse_pos_callback(window_mouse_pos_callback value) noexcept final { return m_impl->set_mouse_pos_callback(value); }
		
		window_position_callback set_position_callback(window_position_callback value) noexcept final { return m_impl->set_position_callback(value); }
		
		window_refresh_callback set_refresh_callback(window_refresh_callback value) noexcept final { return m_impl->set_refresh_callback(value); }
		
		window_resize_callback set_resize_callback(window_resize_callback value) noexcept final { return m_impl->set_resize_callback(value); }
		
		window_scroll_callback set_scroll_callback(window_scroll_callback value) noexcept final { return m_impl->set_scroll_callback(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		scary<base_window> m_impl; // backend implementation

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NATIVE_WINDOW_HPP_