#ifndef _ML_WINDOW_CONTEXT_HPP_
#define _ML_WINDOW_CONTEXT_HPP_

#include <detail/Timer.hpp>
#include <system/Memory.hpp>
#include <window/Cursor.hpp>
#include <window/Monitor.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// window handle
	ML_decl_handle(window_handle);

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
		if (j.is_number()) { v = (context_api_)j.get<int32_t>(); }
		else if (j.is_string())
		{
			switch (auto const s{ j.get<pmr::string>() }; hashof(util::to_lower(s)))
			{
			case hashof("unknown"	): v = context_api_unknown	; break;
			case hashof("opengl"	): v = context_api_opengl	; break;
			case hashof("vulkan"	): v = context_api_vulkan	; break;
			case hashof("directx"	): v = context_api_directx	; break;
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
		if (j.is_number()) { v = (context_profile_)j.get<int32_t>(); }
		else if (j.is_string())
		{
			switch (auto const s{ j.get<pmr::string>() }; hashof(util::to_lower(s)))
			{
			case hashof("any"	): v = context_profile_any		; break;
			case hashof("core"	): v = context_profile_core		; break;
			case hashof("compat"): v = context_profile_compat	; break;
			case hashof("debug"	): v = context_profile_debug	; break;
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
}

#endif // !_ML_WINDOW_CONTEXT_HPP_