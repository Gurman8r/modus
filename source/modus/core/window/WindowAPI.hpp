#ifndef _ML_WINDOW_API_HPP_
#define _ML_WINDOW_API_HPP_

#include <core/system/Timer.hpp>
#include <core/window/Cursor.hpp>
#include <core/window/Input.hpp>
#include <core/window/Monitor.hpp>
#include <core/window/VideoMode.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// window handle
	ML_decl_handle(window_handle);

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

	// context api
	enum context_api_ : int32_t
	{
		context_api_unknown		, // unknown
		context_api_opengl		, // opengl
		context_api_vulkan		, // vulkan
		context_api_directx		, // directx
	};

	inline void from_json(json const & j, context_api_ & v)
	{
		if (j.is_number())
		{
			v = (context_api_)j.get<int32_t>();
		}
		else if (j.is_string())
		{
			using namespace util;
			switch (auto const s{ j.get<pmr::string>() }; hash(to_lower(s)))
			{
			case hash("unknown"	): v = context_api_unknown	; break;
			case hash("opengl"	): v = context_api_opengl	; break;
			case hash("vulkan"	): v = context_api_vulkan	; break;
			case hash("directx"	): v = context_api_directx	; break;
			}
		}
	}

	inline void to_json(json & j, context_api_ const & v)
	{
		switch (v)
		{
		case context_api_unknown: j = "unknown"	; break;
		case context_api_opengl	: j = "opengl"	; break;
		case context_api_vulkan	: j = "vulkan"	; break;
		case context_api_directx: j = "directx"	; break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// context profile
	enum context_profile_ : int32_t
	{
		context_profile_any		, // any
		context_profile_core	, // core
		context_profile_compat	, // compat
		context_profile_debug	, // debug
	};

	inline void from_json(json const & j, context_profile_ & v)
	{
		if (j.is_number())
		{
			v = (context_profile_)j.get<int32_t>();
		}
		else if (j.is_string())
		{
			using namespace util;
			switch (auto const s{ j.get<pmr::string>() }; hash(to_lower(s)))
			{
			case hash("any"		): v = context_profile_any		; break;
			case hash("core"	): v = context_profile_core		; break;
			case hash("compat"	): v = context_profile_compat	; break;
			case hash("debug"	): v = context_profile_debug	; break;
			}
		}
	}

	inline void to_json(json & j, context_profile_ const & v)
	{
		switch (v)
		{
		case context_profile_any	: j = "any"		; break;
		case context_profile_core	: j = "core"	; break;
		case context_profile_compat	: j = "compat"	; break;
		case context_profile_debug	: j = "debug"	; break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// context settings
	struct ML_NODISCARD context_settings final
	{
		int32_t		api				{ context_api_unknown };
		int32_t		major			{};
		int32_t		minor			{};
		int32_t		profile			{ context_profile_any };
		int32_t		depth_bits		{};
		int32_t		stencil_bits	{};
		bool		multisample		{};
		bool		srgb_capable	{};
	};

	inline void from_json(json const & j, context_settings & v)
	{
		j["api"			].get_to((context_api_ & )v.api);
		j["major"		].get_to(v.major);
		j["minor"		].get_to(v.minor);
		j["profile"		].get_to((context_profile_ &)v.profile);
		j["depth_bits"	].get_to(v.depth_bits);
		j["stencil_bits"].get_to(v.stencil_bits);
		j["multisample"	].get_to(v.multisample);
		j["srgb_capable"].get_to(v.srgb_capable);
	}

	inline void to_json(json & j, context_settings const & v)
	{
		j["api"			] = (context_api_)v.api;
		j["major"		] = v.major;
		j["minor"		] = v.minor;
		j["profile"		] = (context_profile_)v.profile;
		j["depth_bits"	] = v.depth_bits;
		j["stencil_bits"] = v.stencil_bits;
		j["multisample"	] = v.multisample;
		j["srgb_capable"] = v.srgb_capable;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// window context
	struct ML_NODISCARD window_context final
	{
		int32_t (*is_extension_supported)(cstring);

		window_handle (*get_context_current)();

		void * (*get_proc_address)(cstring);

		pmr::vector<monitor_handle> const & (*get_monitors)();

		monitor_handle (*get_primary_monitor)();

		duration (*get_time)();

		void (*make_context_current)(window_handle);

		void (*poll_events)();

		void (*swap_buffers)(window_handle);

		void (*swap_interval)(int32_t);

		cursor_handle (*create_custom_cursor)(size_t, size_t, byte_t const *);

		cursor_handle (*create_standard_cursor)(int32_t);

		void (*destroy_cursor)(cursor_handle);
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// window settings
	struct ML_NODISCARD window_settings final
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