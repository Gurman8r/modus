#include <modus_core/engine/Application.hpp>
#include <modus_core/embed/Python.hpp>

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
	memory_manager						mman{ &view };

	memcfg() noexcept { pmr::set_default_resource(mman.get_resource()); }

	~memcfg() noexcept { pmr::set_default_resource(nullptr); }

} const & ML_anon{ memcfg::get_singleton() };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SETTINGS_PATH
#define SETTINGS_PATH "../../../resource/modus_launcher.json"
#endif

static auto const default_settings{ R"(
{
	"app_name": "modus launcher",
	"app_version": "alpha",
	"app_data_path": "../../../",
	"library_paths": [ "../../../" ],

	"window": {
		"title": "modus",
		"video": {
			"resolution": [ 1280, 720 ],
			"bits_per_pixel": [ 8, 8, 8, 8 ],
			"refresh_rate": -1
		},
		"graphics": {
			"api": "opengl",
			"major": 4,
			"minor": 6,
			"profile": "compat",
			"depth_bits": 24,
			"stencil_bits": 8,
			"multisample": true,
			"srgb_capable": false
		},
		"hints": {
			"auto_iconify": true,
			"decorated": true,
			"doublebuffer": false,
			"center_cursor": false,
			"floating": false,
			"focus_on_show": true,
			"focused": true,
			"maximized": true,
			"resizable": true,
			"visible": false
		}
	},

	"imgui": {
		"style": { "path": "resource/modus_launcher.style" }
	},

	"plugins": [
		{ "path": "plugins/sandbox" }
	],

	"scripts": [
		{ "path": "resource/modus_launcher.py" }
	]
}
)"_json };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 main(int32 argc, char * argv[])
{
	application app{ argc, argv, util::load_json(SETTINGS_PATH, default_settings) };

	if (!app.has_interpreter()) {
		ML_assert(app.initialize_interpreter());
	}

	if (app.has_attr("plugins")) {
		for (json const & e : app.attr("plugins")) {
			if (e.contains("path")) {
				app.get_plugin_manager()->install(e["path"]);
			}
		}
	}

	if (app.has_attr("scripts")) {
		for (json const & e : app.attr("scripts")) {
			if (e.contains("path")) {
				app.get_interpreter()->eval_file(app.path_to(e["path"]));
			}
		}
	}

	return app.exec();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */