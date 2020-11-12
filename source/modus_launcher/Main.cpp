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
	},
	"plugins": [
		{ "path": "./plugins/sandbox" }
	],
	"scripts": [
		{ "path": "resource/modus_launcher.py" }
	]
}
)"_json };

json load_settings(fs::path const & path = SETTINGS_PATH) noexcept
{
	std::ifstream f{ path }; ML_defer(&f) { f.close(); };

	return f ? json::parse(f) : default_settings;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t main(int32_t argc, char * argv[])
{
	static memory_manager memory{};

	json prefs{ load_settings() };

	auto app{ make_scope<application>(argc, argv) };
	app->set_app_name("modus launcher");
	app->set_app_version("alpha");
	app->set_library_paths({ "../../../" });

	auto const bus		{ app->get_bus() };
	auto const loop		{ app->get_main_loop() };
	auto const win		{ app->get_window() };
	auto const imgui	{ win->get_imgui() };
	auto const docker	{ win->get_dockspace() };

	loop->set_loop_condition(&main_window::is_open, win);
	
	loop->set_enter_callback([&]()
	{
		// path
		ML_assert(prefs.contains("path"));
		auto const app_path{ prefs["path"].get<ds::string>() };

		// python
		ML_assert(app->initialize_interpreter());

		// window
		ML_assert(prefs.contains("window"));
		auto & window_prefs{ prefs["window"] };
		ML_assert(win->open(
			window_prefs["title"],
			window_prefs["video"],
			window_prefs["context"],
			window_prefs["hints"]));

		// imgui
		ML_assert(prefs.contains("imgui"));
		auto & gui_prefs{ prefs["imgui"] };
		if (gui_prefs.contains("dockspace")) {
			auto & dock_prefs{ gui_prefs["dockspace"] };
			dock_prefs["alpha"].get_to(docker->Alpha);
			dock_prefs["border"].get_to(docker->Border);
			dock_prefs["padding"].get_to(docker->Padding);
			dock_prefs["rounding"].get_to(docker->Rounding);
			dock_prefs["size"].get_to(docker->Size);
		}
		if (gui_prefs.contains("style")) {
			auto & style_prefs{ gui_prefs["style"] };
			if (style_prefs.is_string())
			{
				ImGui_LoadStyle(app_path + style_prefs.get<ds::string>());
			}
		}

		// scripts
		if (prefs.contains("scripts")) {
			for (auto const & e : prefs["scripts"]) {
				auto const path{ app_path + e["path"].get<ds::string>() };
				PyRun_AnyFileEx(std::fopen(path.c_str(), "r"), path.c_str(), true);
			}
		}

		bus->fire<app_enter_event>();
	});
	
	loop->set_exit_callback([&]()
	{
		bus->fire<app_exit_event>();

		ML_assert(app->finalize_interpreter());
	});
	
	loop->set_idle_callback([&]()
	{
		win->get_window_manager()->poll_events();

		bus->fire<app_idle_event>();

		win->do_imgui_frame();
	});

	// demo dummy
	enum
	{
		viewport_panel,
		MAX_PANEL
	};
	ImGuiExt::Panel m_panels[MAX_PANEL]
	{
		{ "viewport", true, ImGuiWindowFlags_MenuBar },
	};
	color m_clear_color{ 0.223f, 0.f, 0.46f, 1.f };
	vec2 m_resolution{ 1280, 720 };
	ds::list<ds::ref<gfx::framebuffer>> m_fb{};
	bus->new_dummy<
		app_enter_event,
		app_exit_event,
		app_idle_event,
		imgui_dockspace_event,
		imgui_render_event
	> ([&](event const & value)
	{
		switch (value)
		{
		case app_enter_event::ID: {
			m_fb.push_back(gfx::make_framebuffer((vec2i)m_resolution));
		} break;

		case app_exit_event::ID: {
			debug::ok("goodbye!");
		} break;

		case app_idle_event::ID: {
			for (auto & fb : m_fb) { fb->resize(m_resolution); }
			win->draw_commands(
				gfx::command::bind_framebuffer(m_fb[0]),
				gfx::command::set_clear_color(m_clear_color),
				gfx::command::clear(gfx::clear_color | gfx::clear_depth),
				gfx::command([&](gfx::render_context * ctx) noexcept { /* custom */ }),
				gfx::command::bind_framebuffer(nullptr));
		} break;

		case imgui_dockspace_event::ID: {
			auto && ev{ (imgui_dockspace_event &&)value };
			if (ImGuiID const root{ ev->GetID() }; !ImGui::DockBuilderGetNode(root))
			{
				ImGui::DockBuilderRemoveNode(root);
				ImGui::DockBuilderAddNode(root, ev->DockFlags);
				ImGui::DockBuilderDockWindow(m_panels[viewport_panel].Title, root);
				ImGui::DockBuilderFinish(root);
			}
		} break;

		case imgui_render_event::ID: {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel]([&]() noexcept
			{
				ImGui::PopStyleVar(1);

				if (ImGui::BeginMenuBar()) {
					ImGuiExt::HelpMarker("viewport");
					ImGui::Separator();

					ImGui::ColorEdit4("clear color", m_clear_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					ImGui::Separator();

					auto const fps{ ImGui::GetIO().Framerate };
					ImGui::TextDisabled("%.3f ms/frame ( %.1f fps )", 1000.f / fps, fps);
					ImGui::Separator();

					ImGui::EndMenuBar();
				}

				ImGui::Image(
					m_fb[0]->get_color_attachments()[0]->get_handle(),
					m_resolution = ImGui::GetContentRegionAvail(),
					{ 0, 1 }, { 1, 0 },
					colors::white,
					colors::clear);
			}))
			{
				ImGui::PopStyleVar(1);
			}
		} break;
		}
	});

	// execute
	return app->exec();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */