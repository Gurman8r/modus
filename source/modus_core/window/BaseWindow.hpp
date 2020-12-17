#ifndef _ML_BASE_WINDOW_HPP_
#define _ML_BASE_WINDOW_HPP_

#include <modus_core/window/WindowContext.hpp>

// BACKEND
namespace ml
{
	// backend window
	struct ML_CORE_API backend_window : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		virtual ~backend_window() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			ds::string const &			title,
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
			return { get_position(), get_size() };
		}

		ML_NODISCARD virtual window_callbacks const & get_callbacks() const = 0;

		ML_NODISCARD virtual ds::string get_clipboard() const = 0;

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

		ML_NODISCARD virtual ds::string const & get_title() const = 0;

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

		virtual void set_clipboard(cstring) = 0;

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

		virtual void set_title(ds::string const &) = 0;

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
			set_scroll_callback				(nullptr);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// FRONTEND
namespace ml
{
	// frontend window
	template <class Derived
	> struct ML_CORE_API frontend_window : backend_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using backend_window::allocator_type;

		explicit frontend_window(backend_window * value) noexcept : m_ptr{ ML_check(value) } {}

		virtual ~frontend_window() noexcept override {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			ds::string const &			title,
			video_mode const &			vm		= {},
			context_settings const &	cs		= {},
			window_hints_				hints	= window_hints_default
		) override
		{
			return m_ptr->open(title, vm, cs, hints);
		}
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void focus() noexcept final { m_ptr->focus(); }

		void hide() noexcept final { m_ptr->hide(); }

		void iconify() noexcept final { m_ptr->iconify(); }

		void maximize() noexcept final { m_ptr->maximize(); }

		void restore() noexcept final { m_ptr->restore(); }

		void request_attention() noexcept final { m_ptr->request_attention(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD window_callbacks const & get_callbacks() const noexcept final { return m_ptr->get_callbacks(); }

		ML_NODISCARD ds::string get_clipboard() const noexcept final { return m_ptr->get_clipboard(); }

		ML_NODISCARD vec2 get_content_scale() const noexcept final { return m_ptr->get_content_scale(); }

		ML_NODISCARD vec2i get_framebuffer_size() const noexcept final { return m_ptr->get_framebuffer_size(); }

		ML_NODISCARD window_handle get_handle() const noexcept final { return m_ptr->get_handle(); }

		ML_NODISCARD window_hints_ get_hints() const noexcept final { return m_ptr->get_hints(); }

		ML_NODISCARD int32 get_input_mode(int32 value) const noexcept final { return m_ptr->get_input_mode(value); }

		ML_NODISCARD int32 get_key(int32 value) const noexcept final { return m_ptr->get_key(value); }

		ML_NODISCARD int32 get_mouse_button(int32 value) const noexcept final { return m_ptr->get_mouse_button(value); }

		ML_NODISCARD vec2 get_mouse_pos() const noexcept final { return m_ptr->get_mouse_pos(); }

		ML_NODISCARD window_handle get_native_handle() const noexcept final { return m_ptr->get_native_handle(); }

		ML_NODISCARD float32 get_opacity() const noexcept final { return m_ptr->get_opacity(); }

		ML_NODISCARD vec2i get_position() const noexcept final { return m_ptr->get_position(); }

		ML_NODISCARD vec2i get_size() const noexcept final { return m_ptr->get_size(); }

		ML_NODISCARD ds::string const & get_title() const noexcept final { return m_ptr->get_title(); }

		ML_NODISCARD void * get_user_pointer() const noexcept final { return m_ptr->get_user_pointer(); }

		ML_NODISCARD int_rect get_window_frame_size() const noexcept final { return m_ptr->get_window_frame_size(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool is_auto_iconify() const noexcept final { return m_ptr->is_auto_iconify(); }

		ML_NODISCARD bool is_decorated() const noexcept final { return m_ptr->is_decorated(); }

		ML_NODISCARD bool is_floating() const noexcept final { return m_ptr->is_floating(); }

		ML_NODISCARD bool is_focus_on_show() const noexcept final { return m_ptr->is_focus_on_show(); }

		ML_NODISCARD bool is_focused() const noexcept final { return m_ptr->is_focused(); }

		ML_NODISCARD bool is_hovered() const noexcept final { return m_ptr->is_hovered(); }

		ML_NODISCARD bool is_iconified() const noexcept final { return m_ptr->is_iconified(); }

		ML_NODISCARD bool is_maximized() const noexcept final { return m_ptr->is_maximized(); }

		ML_NODISCARD bool is_open() const noexcept final { return m_ptr->is_open(); }

		ML_NODISCARD bool is_resizable() const noexcept final { return m_ptr->is_resizable(); }

		ML_NODISCARD bool is_transparent() const noexcept final { return m_ptr->is_transparent(); }

		ML_NODISCARD bool is_visible() const noexcept final { return m_ptr->is_visible(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_auto_iconify(bool value) noexcept final { m_ptr->set_auto_iconify(value); }

		void set_clipboard(cstring value) noexcept final { m_ptr->set_clipboard(value); }
		
		void set_cursor(cursor_handle value) noexcept final { m_ptr->set_cursor(value); }
		
		void set_cursor_mode(int32 value) noexcept final { m_ptr->set_cursor_mode(value); }
		
		void set_mouse_pos(vec2d const & value) noexcept final { m_ptr->set_mouse_pos(value); }

		void set_decorated(bool value) noexcept final { m_ptr->set_decorated(value); }

		void set_floating(bool value) noexcept final { m_ptr->set_floating(value); }

		void set_focus_on_show(bool value) noexcept final { m_ptr->set_focus_on_show(value); }
		
		void set_icons(int32 n, size_t w, size_t h, byte const * p) noexcept final { m_ptr->set_icons(n, w, h, p); }

		void set_input_mode(int32 mode, int32 value) noexcept final { m_ptr->set_input_mode(mode, value); }
		
		void set_opacity(float32 value) noexcept final { m_ptr->set_opacity(value); }

		void set_position(vec2i const & value) noexcept final { m_ptr->set_position(value); }
		
		void set_monitor(monitor_handle value, int_rect const & bounds = {}) noexcept final { m_ptr->set_monitor(value, bounds); }

		void set_resizable(bool value) noexcept final { m_ptr->set_resizable(value); }

		void set_should_close(bool value) noexcept final { m_ptr->set_should_close(value); }

		void set_size(vec2i const & value) noexcept final { m_ptr->set_size(value); }
		
		void set_title(ds::string const & value) noexcept final { m_ptr->set_title(value); }

		void set_user_pointer(void * value) noexcept final { m_ptr->set_user_pointer(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD window_char_callback get_char_callback() const noexcept final { return m_ptr->get_char_callback(); }
		
		ML_NODISCARD window_char_mods_callback get_char_mods_callback() const noexcept final { return m_ptr->get_char_mods_callback(); }
		
		ML_NODISCARD window_close_callback get_close_callback() const noexcept final { return m_ptr->get_close_callback(); }
		
		ML_NODISCARD window_content_scale_callback get_content_scale_callback() const noexcept final { return m_ptr->get_content_scale_callback(); }
		
		ML_NODISCARD window_drop_callback get_drop_callback() const noexcept final { return m_ptr->get_drop_callback(); }
		
		ML_NODISCARD window_focus_callback get_focus_callback() const noexcept final { return m_ptr->get_focus_callback(); }
		
		ML_NODISCARD window_framebuffer_resize_callback get_framebuffer_resize_callback() const noexcept final { return m_ptr->get_framebuffer_resize_callback(); }
		
		ML_NODISCARD window_iconify_callback get_iconify_callback() const noexcept final { return m_ptr->get_iconify_callback(); }
		
		ML_NODISCARD window_key_callback get_key_callback() const noexcept final { return m_ptr->get_key_callback(); }
		
		ML_NODISCARD window_maximize_callback get_maximize_callback() const noexcept final { return m_ptr->get_maximize_callback(); }
		
		ML_NODISCARD window_mouse_button_callback get_mouse_button_callback() const noexcept final { return m_ptr->get_mouse_button_callback(); }

		ML_NODISCARD window_mouse_enter_callback get_mouse_enter_callback() const noexcept final { return m_ptr->get_mouse_enter_callback(); }

		ML_NODISCARD window_mouse_pos_callback get_mouse_pos_callback() const noexcept final { return m_ptr->get_mouse_pos_callback(); }
		
		ML_NODISCARD window_position_callback get_position_callback() const noexcept final { return m_ptr->get_position_callback(); }
		
		ML_NODISCARD window_refresh_callback get_refresh_callback() const noexcept final { return m_ptr->get_refresh_callback(); }
		
		ML_NODISCARD window_resize_callback get_resize_callback() const noexcept final { return m_ptr->get_resize_callback(); }
		
		ML_NODISCARD window_scroll_callback get_scroll_callback() const noexcept final { return m_ptr->get_scroll_callback(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		window_char_callback set_char_callback(window_char_callback value) noexcept final { return m_ptr->set_char_callback(value); }
		
		window_char_mods_callback set_char_mods_callback(window_char_mods_callback value) noexcept final { return m_ptr->set_char_mods_callback(value); }
		
		window_close_callback set_close_callback(window_close_callback value) noexcept final { return m_ptr->set_close_callback(value); }
		
		window_content_scale_callback set_content_scale_callback(window_content_scale_callback value) noexcept final { return m_ptr->set_content_scale_callback(value); }
		
		window_drop_callback set_drop_callback(window_drop_callback value) noexcept final { return m_ptr->set_drop_callback(value); }
		
		window_focus_callback set_focus_callback(window_focus_callback value) noexcept final { return m_ptr->set_focus_callback(value); }
		
		window_framebuffer_resize_callback set_framebuffer_resize_callback(window_framebuffer_resize_callback value) noexcept final { return m_ptr->set_framebuffer_resize_callback(value); }
		
		window_iconify_callback set_iconify_callback(window_iconify_callback value) noexcept final { return m_ptr->set_iconify_callback(value); }
		
		window_key_callback set_key_callback(window_key_callback value) noexcept final { return m_ptr->set_key_callback(value); }
		
		window_maximize_callback set_maximize_callback(window_maximize_callback value) noexcept final { return m_ptr->set_maximize_callback(value); }
		
		window_mouse_button_callback set_mouse_button_callback(window_mouse_button_callback value) noexcept final { return m_ptr->set_mouse_button_callback(value); }

		window_mouse_enter_callback set_mouse_enter_callback(window_mouse_enter_callback value) noexcept final { return m_ptr->set_mouse_enter_callback(value); }

		window_mouse_pos_callback set_mouse_pos_callback(window_mouse_pos_callback value) noexcept final { return m_ptr->set_mouse_pos_callback(value); }
		
		window_position_callback set_position_callback(window_position_callback value) noexcept final { return m_ptr->set_position_callback(value); }
		
		window_refresh_callback set_refresh_callback(window_refresh_callback value) noexcept final { return m_ptr->set_refresh_callback(value); }
		
		window_resize_callback set_resize_callback(window_resize_callback value) noexcept final { return m_ptr->set_resize_callback(value); }
		
		window_scroll_callback set_scroll_callback(window_scroll_callback value) noexcept final { return m_ptr->set_scroll_callback(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scope<backend_window> m_ptr; // backend implementation

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_BASE_WINDOW_HPP_