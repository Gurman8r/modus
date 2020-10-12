#ifndef _ML_WINDOW_EVENTS_HPP_
#define _ML_WINDOW_EVENTS_HPP_

#include <modus_core/system/Events.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_char_event)
	{
		uint32_t const value;

		constexpr window_char_event(uint32_t value) noexcept
			: value{ value }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_char_mods_event)
	{
		uint32_t const value;
		int32_t const mods;

		constexpr window_char_mods_event(uint32_t value, int32_t mods) noexcept
			: value{ value }, mods{ mods }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_close_event)
	{

		constexpr window_close_event() noexcept
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_content_scale_event)
	{
		float_t const x, y;

		constexpr window_content_scale_event(float_t x, float_t y) noexcept
			: x{ x }, y{ y }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_drop_event)
	{
		int32_t const count;
		cstring const * const paths;

		constexpr window_drop_event(int32_t count, cstring const * paths) noexcept
			: count{ count }, paths{ paths }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_cursor_enter_event)
	{
		int32_t const entered;

		constexpr window_cursor_enter_event(int32_t entered) noexcept
			: entered{ entered }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_cursor_pos_event)
	{
		float64_t const x, y;

		constexpr window_cursor_pos_event(float64_t x, float64_t y) noexcept
			: x{ x }, y{ y }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_error_event)
	{
		int32_t const code;
		cstring const desc;

		constexpr window_error_event(int32_t code, cstring desc) noexcept
			: code{ code }, desc{ desc }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_focus_event)
	{
		int32_t const focused;

		constexpr window_focus_event(int32_t focused) noexcept
			: focused{ focused }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_framebuffer_resize_event)
	{
		int32_t const width, height;

		constexpr window_framebuffer_resize_event(int32_t width, int32_t height) noexcept
			: width{ width }, height{ height }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_iconify_event)
	{
		int32_t const iconified;

		constexpr window_iconify_event(int32_t iconified) noexcept
			: iconified{ iconified }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_key_event)
	{
		int32_t const key, scan, action, mods;

		constexpr window_key_event(int32_t key, int32_t scan, int32_t action, int32_t mods) noexcept
			: key{ key }, scan{ scan }, action{ action }, mods{ mods }
		{
		}

		constexpr bool is_action	(int32_t a) const noexcept { return action == a; }
		constexpr bool is_key		(int32_t k) const noexcept { return key == k; }
		constexpr bool is_mods		(int32_t m) const noexcept { return mods == m; }
		constexpr bool has_mods		(int32_t m) const noexcept { return (m & mods) == m; }
		
		constexpr bool is_release	(int32_t k) const noexcept { return is_key(k) && is_action(input_state_release); }
		constexpr bool is_press		(int32_t k) const noexcept { return is_key(k) && is_action(input_state_press); }
		constexpr bool is_repeat	(int32_t k) const noexcept { return is_key(k) && is_action(input_state_repeat); }

		constexpr bool is_release	(int32_t k, int32_t m) const noexcept { return is_release(k) && is_mods(m); }
		constexpr bool is_press		(int32_t k, int32_t m) const noexcept { return is_press(k) && is_mods(m); }
		constexpr bool is_repeat	(int32_t k, int32_t m) const noexcept { return is_repeat(k) && is_mods(m); }

		constexpr bool is_shift		() const noexcept { return has_mods(key_mods_shift);  }
		constexpr bool is_ctrl		() const noexcept { return has_mods(key_mods_ctrl); }
		constexpr bool is_alt		() const noexcept { return has_mods(key_mods_alt); }
		constexpr bool is_super		() const noexcept { return has_mods(key_mods_super); }
		constexpr bool is_caps		() const noexcept { return has_mods(key_mods_caps_lock); }
		constexpr bool is_numlk		() const noexcept { return has_mods(key_mods_num_lock); }

		constexpr bool is_shift		(int32_t k) const noexcept { return is_press(k) && is_shift();  }
		constexpr bool is_ctrl		(int32_t k) const noexcept { return is_press(k) && is_ctrl(); }
		constexpr bool is_alt		(int32_t k) const noexcept { return is_press(k) && is_alt(); }
		constexpr bool is_super		(int32_t k) const noexcept { return is_press(k) && is_super(); }
		constexpr bool is_caps		(int32_t k) const noexcept { return is_press(k) && is_caps(); }
		constexpr bool is_numlk		(int32_t k) const noexcept { return is_press(k) && is_numlk(); }

		constexpr bool is_new		() const noexcept { return is_ctrl(key_code_n); }
		constexpr bool is_open		() const noexcept { return is_ctrl(key_code_o); }
		constexpr bool is_save		() const noexcept { return is_ctrl(key_code_s) || is_press(key_code_s, key_mods_ctrl | key_mods_shift); }
		constexpr bool is_undo		() const noexcept { return is_ctrl(key_code_z); }
		constexpr bool is_redo		() const noexcept { return is_ctrl(key_code_y) || is_press(key_code_z, key_mods_ctrl | key_mods_shift); }
		constexpr bool is_cut		() const noexcept { return is_ctrl(key_code_x) || is_shift(key_code_del); }
		constexpr bool is_copy		() const noexcept { return is_ctrl(key_code_c) || is_ctrl(key_code_insert); }
		constexpr bool is_paste		() const noexcept { return is_ctrl(key_code_v) || is_shift(key_code_insert); }
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_maximize_event)
	{
		int32_t const maximized;

		constexpr window_maximize_event(int32_t maximized) noexcept
			: maximized{ maximized }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_mouse_event)
	{
		int32_t const button, action, mods;

		constexpr window_mouse_event(int32_t button, int32_t action, int32_t mods) noexcept
			: button{ button }, action{ action }, mods{ mods }
		{
		}

		constexpr bool is_action	(int32_t a) const noexcept { return action == a; }
		constexpr bool is_button	(int32_t b) const noexcept { return button == b; }
		constexpr bool is_mods		(int32_t m) const noexcept { return mods == m; }
		constexpr bool has_mods		(int32_t m) const noexcept { return (m & mods) == m; }
		
		constexpr bool is_release	(int32_t b) const noexcept { return is_button(b) && is_action(input_state_release); }
		constexpr bool is_press		(int32_t b) const noexcept { return is_button(b) && is_action(input_state_press); }
		constexpr bool is_repeat	(int32_t b) const noexcept { return is_button(b) && is_action(input_state_repeat); }

		constexpr bool is_release	(int32_t b, int32_t m) const noexcept { return is_release(b) && is_mods(m); }
		constexpr bool is_press		(int32_t b, int32_t m) const noexcept { return is_press(b) && is_mods(m); }
		constexpr bool is_repeat	(int32_t b, int32_t m) const noexcept { return is_repeat(b) && is_mods(m); }
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_position_event)
	{
		int32_t const x, y;

		constexpr window_position_event(int32_t x, int32_t y) noexcept
			: x{ x }, y{ y }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_refresh_event)
	{
		constexpr window_refresh_event() noexcept
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_scroll_event)
	{
		float64_t const x, y;

		constexpr window_scroll_event(float64_t x, float64_t y) noexcept
			: x{ x }, y{ y }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_event(window_resize_event)
	{
		int32_t const width, height;

		constexpr window_resize_event(int32_t width, int32_t height) noexcept
			: width{ width }, height{ height }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_WINDOW_EVENTS_HPP_