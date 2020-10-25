#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientDatabase.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

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
		"style": "resource/modus_launcher.style",
		"dockspace": {
			"alpha"		: 0,
			"border"	: 0,
			"padding"	: [ 0, 0 ],
			"rounding"	: 0,
			"size"		: [ 0, 0 ]
		}
	},
	"client": {
		"callbacks": true,
		"plugins": [
			{ "path": "./plugins/sandbox" }
		],
		"scripts": [
			{ "path": "resource/modus_launcher.py" }
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
	static memory_manager	mem		{};
	static client_io		io		{ __argc, __argv, load_settings() };
	static event_bus		bus		{};
	static render_window	win		{};
	static client_database	db		{};
	static client_context	ctx		{ &mem, &io, &bus, &win, &db };
	static client_runtime	runtime	{ &ctx };

	return set_default_runtime(&runtime)->idle();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */