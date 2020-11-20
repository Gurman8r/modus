#include <modus_core/engine/Application.hpp>

using namespace ml;
using namespace ml::byte_literals;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef RESERVE_MEMORY
#define RESERVE_MEMORY 128_MiB
#endif

static class memcfg final : public singleton<memcfg>
{
	friend singleton;

	ds::array<byte, RESERVE_MEMORY>		data{};
	pmr::monotonic_buffer_resource		mono{ data.data(), data.size() };
	pmr::unsynchronized_pool_resource	pool{ &mono };
	passthrough_resource				view{ &pool, data.data(), data.size() };
	memory_manager						mman{ pmr::set_default_resource(&view) };

	~memcfg() noexcept { pmr::set_default_resource(nullptr); }

} const & ML_anon{ memcfg::get_singleton() };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SETTINGS_PATH
#define SETTINGS_PATH "../../../resource/modus_launcher.json"
#endif

static auto const default_settings{ R"(
{
	"app_name"		: "modus launcher",
	"app_version"	: "alpha",
	"arguments"		: [],
	"library_paths" : [ "../../../" ],

	"main_window": {
		"video_mode": {
			"resolution"	: [ 1280, 720 ],
			"bits_per_pixel": [ 8, 8, 8, 8 ],
			"refresh_rate"	: -1
		},
		"context_settings": {
			"api"			: "opengl",
			"major"			: 4,
			"minor"			: 6,
			"profile"		: "compat",
			"depth_bits"	: 24,
			"stencil_bits"	: 8,
			"multisample"	: true,
			"srgb_capable"	: false
		},
		"window_hints": {
			"auto_iconify"	: true,
			"decorated"		: true,
			"doublebuffer"	: false,
			"center_cursor"	: false,
			"floating"		: false,
			"focus_on_show"	: true,
			"focused"		: true,
			"maximized"		: true,
			"resizable"		: true,
			"visible"		: false
		},
		"imgui_theme": {
			"path": "resource/modus_launcher.style"
		},
		"imgui_dockspace": {
			"alpha"		: 0,
			"border"	: 0,
			"padding"	: [ 0, 0 ],
			"rounding"	: 0,
			"size"		: [ 0, 0 ]
		}
	},

	"plugins": [
		{ "path": "./plugins/sandbox" }
	],

	"scripts": [
		{ "path": "resource/modus_launcher.py" }
	]
}
)"_json };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 main(int32 argc, char * argv[])
{
	auto load_settings = [](fs::path const & path = SETTINGS_PATH)
	{
		std::ifstream f{ path };
		ML_defer(&f) { f.close(); };
		return f ? json::parse(f) : default_settings;
	};

	application app{ argc, argv };
	app.set_attributes(load_settings());
	app.set_app_name(app.attr("app_name"));
	app.set_app_version(app.attr("app_version"));
	app.set_arguments(app.attr("arguments"));
	app.set_library_paths(app.attr("library_paths"));
	return app.exec();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */