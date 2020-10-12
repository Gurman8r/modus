#include <modus_core/client/Blackboard.hpp>
#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiRuntime.hpp>
#include <modus_core/window/WindowEvents.hpp>
#include <modus_core/scene/Node.hpp>

using namespace ml;
using namespace ml::byte_literals;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define RESERVE_MEMORY 128_MiB

static class memcfg final : public singleton<memcfg>
{
	friend singleton;

	ds::array<byte_t, RESERVE_MEMORY>	data{};
	pmr::monotonic_buffer_resource		mono{ data.data(), data.size() };
	pmr::unsynchronized_pool_resource	pool{ &mono };
	passthrough_resource				view{ &pool, data.data(), data.size() };

	memcfg() noexcept { pmr::set_default_resource(&view); }

	~memcfg() noexcept { pmr::set_default_resource(nullptr); }

} const & ML_anon{ *memcfg::get_singleton() };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define SETTINGS_PATH "../../../resource/modus_launcher.json"

static auto const default_settings{ R"(
{
	"path": "../../../",
	"window": {
		"title": "modus",
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
		"inst_clbk"		: true,
		"style_path"	: "assets/styles/obsidian.style",
		"menubar"		: { "enabled": true },
		"dockspace"		: {
			"enabled"	: true,
			"title"		: "dockspace##libmeme",
			"border"	: 0,
			"rounding"	: 0,
			"alpha"		: 0,
			"padding"	: [ 0, 0 ],
			"size"		: [ 0, 0 ],
			"nodes"		: []
		}
	},
	"plugins": {
		"files": [
			"plugins/sandbox"
		]
	},
	"scripts": {
		"files": [
			"assets/scripts/setup.py"
		]
	}
}
)"_json };

static auto load_settings(fs::path const & path = SETTINGS_PATH)
{
	std::ifstream f{ path }; ML_defer(&f) { f.close(); };

	return f ? json::parse(f) : default_settings;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

ml::int32_t main()
{
	// client
	static memory_manager	mem		{ pmr::get_default_resource() };
	static client_io		io		{ __argc, __argv, mem.get_allocator(), load_settings() };
	static blackboard		vars	{ io.alloc };
	static event_bus		bus		{ io.alloc };
	static render_window	win		{ io.alloc };
	static imgui_runtime	gui		{ &bus, &win, io.alloc };
	static client_context	context	{ &mem, &io, &vars, &bus, &win, &gui };
	static client_runtime	runtime	{ &context };

	node root{
		new node{
		},
		new node{
		},
		new node{
		},
		new node{
		},
	};

	// install plugins
	for (auto const & path : io.prefs["plugins"]["files"])
	{
		runtime.get_plugins().install(path);
	}
	
	// execute scripts
	for (auto const & path : io.prefs["scripts"]["files"])
	{
		py::eval_file(io.path2(path).string());
	}

	// idle
	return runtime.idle();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */