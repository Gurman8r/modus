#ifndef _ML_WINDOW_API_HPP_
#define _ML_WINDOW_API_HPP_

#include <detail/Duration.hpp>
#include <window/Input.hpp>
#include <window/VideoMode.hpp>
#include <window/WindowContext.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// callback types
	ML_alias window_char_callback				= void(*)(window_handle, uint32_t);
	ML_alias window_char_mods_callback			= void(*)(window_handle, uint32_t, int32_t);
	ML_alias window_close_callback				= void(*)(window_handle);
	ML_alias window_content_scale_callback		= void(*)(window_handle, float_t, float_t);
	ML_alias window_cursor_enter_callback		= void(*)(window_handle, int32_t);
	ML_alias window_cursor_position_callback	= void(*)(window_handle, float64_t, float64_t);
	ML_alias window_drop_callback				= void(*)(window_handle, int32_t, cstring[]);
	ML_alias window_error_callback				= void(*)(int32_t, cstring);
	ML_alias window_focus_callback				= void(*)(window_handle, int32_t);
	ML_alias window_framebuffer_resize_callback	= void(*)(window_handle, int32_t, int32_t);
	ML_alias window_iconify_callback			= void(*)(window_handle, int32_t);
	ML_alias window_key_callback				= void(*)(window_handle, int32_t, int32_t, int32_t, int32_t);
	ML_alias window_maximize_callback			= void(*)(window_handle, int32_t);
	ML_alias window_mouse_callback				= void(*)(window_handle, int32_t, int32_t, int32_t);
	ML_alias window_position_callback			= void(*)(window_handle, int32_t, int32_t);
	ML_alias window_refresh_callback			= void(*)(window_handle);
	ML_alias window_resize_callback				= void(*)(window_handle, int32_t, int32_t);
	ML_alias window_scroll_callback				= void(*)(window_handle, float64_t, float64_t);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// callback container
	struct ML_NODISCARD window_callbacks final
	{
		window_char_callback				on_char					; // 
		window_char_mods_callback			on_char_mods			; // 
		window_close_callback				on_close				; // 
		window_content_scale_callback		on_content_scale		; // 
		window_cursor_enter_callback		on_cursor_enter			; // 
		window_cursor_position_callback		on_cursor_position		; // 
		window_drop_callback				on_drop					; // 
		window_error_callback				on_error				; // 
		window_focus_callback				on_focus				; // 
		window_framebuffer_resize_callback	on_framebuffer_resize	; // 
		window_iconify_callback				on_iconify				; // 
		window_key_callback					on_key					; // 
		window_maximize_callback			on_maximize				; // 
		window_mouse_callback				on_mouse				; // 
		window_position_callback			on_position				; // 
		window_refresh_callback				on_refresh				; // 
		window_resize_callback				on_resize				; // 
		window_scroll_callback				on_scroll				; // 
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// window hints
	enum window_hints_ : int32_t
	{
		window_hints_none			= 0		,		// none
		window_hints_auto_iconify	= 1 << 0,		// auto iconify
		window_hints_center_cursor	= 1 << 1,		// center cursor
		window_hints_decorated		= 1 << 2,		// decorated
		window_hints_doublebuffer	= 1 << 3,		// doublebuffer
		window_hints_floating		= 1 << 4,		// floating
		window_hints_focus_on_show	= 1 << 5,		// focus on show
		window_hints_focused		= 1 << 6,		// focused
		window_hints_maximized		= 1 << 7,		// maximized
		window_hints_resizable		= 1 << 8,		// resizable
		window_hints_visible		= 1 << 9,		// visible

		window_hints_default
			= window_hints_auto_iconify
			| window_hints_decorated
			| window_hints_doublebuffer
			| window_hints_focus_on_show
			| window_hints_focused
			| window_hints_resizable
			| window_hints_visible,

		window_hints_default_max
			= window_hints_auto_iconify
			| window_hints_decorated
			| window_hints_doublebuffer
			| window_hints_focus_on_show
			| window_hints_focused
			| window_hints_maximized
			| window_hints_resizable,
	};

	inline void from_json(json const & j, window_hints_ & v)
	{
		ML_flag_write((int32_t &)v, window_hints_auto_iconify	, j["auto_iconify"	].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_center_cursor	, j["center_cursor"	].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_decorated		, j["decorated"		].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_doublebuffer	, j["doublebuffer"	].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_floating		, j["floating"		].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_focus_on_show	, j["focus_on_show"	].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_focused		, j["focused"		].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_maximized		, j["maximized"		].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_resizable		, j["resizable"		].get<bool>());
		ML_flag_write((int32_t &)v, window_hints_visible		, j["visible"		].get<bool>());
	}

	inline void to_json(json & j, window_hints_ const & v)
	{
		j["auto_iconify"	] = ML_flag_read(v, window_hints_auto_iconify	);
		j["decorated"		] = ML_flag_read(v, window_hints_decorated		);
		j["doublebuffer"	] = ML_flag_read(v, window_hints_doublebuffer	);
		j["center_cursor"	] = ML_flag_read(v, window_hints_center_cursor	);
		j["floating"		] = ML_flag_read(v, window_hints_floating		);
		j["focus_on_show"	] = ML_flag_read(v, window_hints_focus_on_show	);
		j["focused"			] = ML_flag_read(v, window_hints_focused		);
		j["maximized"		] = ML_flag_read(v, window_hints_maximized		);
		j["resizable"		] = ML_flag_read(v, window_hints_resizable		);
		j["visible"			] = ML_flag_read(v, window_hints_visible		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// window settings
	struct ML_NODISCARD window_settings
	{
		pmr::string			title	{ "modus" };
		video_mode			video	{};
		context_settings	context	{};
		int32_t				hints	{ window_hints_default };
	};

	inline void from_json(json const & j, window_settings & v)
	{
		j["title"	].get_to(v.title);
		j["video"	].get_to(v.video);
		j["context"	].get_to(v.context);
		j["hints"	].get_to((window_hints_ &)v.hints);
	}

	inline void to_json(json & j, window_settings const & v)
	{
		j["title"	] = v.title;
		j["video"	] = v.video;
		j["context"	] = v.context;
		j["hints"	] = (window_hints_)v.hints;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_WINDOW_API_HPP_