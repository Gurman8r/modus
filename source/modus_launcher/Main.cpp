#include <modus_core/engine/Application.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/engine/PluginManager.hpp>

using namespace ml;
using namespace ml::byte_literals;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef RESERVE_MEMORY
#define RESERVE_MEMORY 128_MiB
#endif

static class memcfg final : public singleton<memcfg>
{
	friend singleton;

	ds::array<byte_t, RESERVE_MEMORY>	data{};
	pmr::monotonic_buffer_resource		mono{ data.data(), data.size() };
	pmr::unsynchronized_pool_resource	pool{ &mono };
	passthrough_resource				view{ &pool, data.data(), data.size() };

	memcfg() noexcept { pmr::set_default_resource(&view); }

	~memcfg() noexcept { pmr::set_default_resource(nullptr); }

} const & ML_anon{ memcfg::get_singleton() };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SETTINGS_PATH
#define SETTINGS_PATH "../../../resource/modus_launcher.json"
#endif

static auto const default_settings{ R"(
{
	"paths": [
		"../../../"
	],
	"plugins": [
		{ "path": "./plugins/sandbox" }
	],
	"scripts": [
		{ "path": "resource/modus_launcher.py" }
	],
	"window": {
		"title": "modus launcher",
		"video": {
			"resolution"	: [ 1280, 720 ],
			"bits_per_pixel": [ 8, 8, 8, 8 ],
			"refresh_rate"	: -1
		},
		"context": {
			"api"			: "opengl",
			"major"			: 4,
			"minor"			: 6,
			"profile"		: "compat",
			"depth_bits"	: 24,
			"stencil_bits"	: 8,
			"multisample"	: true,
			"srgb_capable"	: false
		},
		"hints": {
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
		}
	},
	"imgui": {
		"style": "resource/modus_launcher.style",
		"dockspace": {
			"alpha"		: 0,
			"border"	: 0,
			"padding"	: [ 0, 0 ],
			"rounding"	: 0,
			"size"		: [ 0, 0 ]
		}
	}
}
)"_json };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t main(int32_t argc, char * argv[])
{
	static memory_manager memory{};

	auto load_settings = [](fs::path const & path = SETTINGS_PATH) noexcept
	{
		std::ifstream f{ path };
		ML_defer(&f) { f.close(); };
		return f ? json::parse(f) : default_settings;
	};

	auto app{ make_scope<application>(argc, argv) };
	app->set_app_name(ML__name);
	app->set_app_version(ML__version);
	app->set_attributes(load_settings());
	app->set_library_paths(app->attr("paths"));

	auto plugs{ make_scope<plugin_manager>(app.get()) };
	if (app->attr().contains("plugins")) {
		for (json const & e : app->attr("plugins")) {
			plugs->install(e["path"]);
		}
	}

	app->get_bus()->new_dummy<app_enter_event>([&](event const & value) {
		if (value == app_enter_event::ID) {
			if (app->attr().contains("scripts")) {
				for (json const & e : app->attr("scripts")) {
					auto const path{ app->path_to(e["path"]).string() };
					PyRun_AnyFileEx(std::fopen(path.c_str(), "r"), path.c_str(), true);
				}
			}
		}
	});

	return app->exec();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */