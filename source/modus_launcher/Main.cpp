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
	"runtime": {
		"callbacks": true,
		"guistyle": "resource/modus_launcher.style",
		"dockspace": {
			"alpha"		: 0,
			"border"	: 0,
			"padding"	: [ 0, 0 ],
			"rounding"	: 0,
			"size"		: [ 0, 0 ]
		},
		"plugins": [
			{ "path": "./plugins/sandbox" }
		],
		"scripts": [
			{ "path": "resource/modus_launcher.py" }
		]
	}
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
	app->set_library_paths({ "./plugins" });

	auto const bus		{ app->get_bus() };
	auto const mainloop	{ app->get_main_loop() };
	auto const window	{ app->get_window() };
	auto const imgui	{ window->get_imgui() };
	auto const docker	{ window->get_docker() };

	mainloop->set_loop_condition(&main_window::is_open, window);
	
	mainloop->set_enter_callback([&]()
	{
		ML_assert(prefs.contains("runtime"));
		auto & runtime_prefs{ prefs["runtime"] };
		bool const install_callbacks{ runtime_prefs["callbacks"] };

		ML_assert(prefs.contains("window"));
		auto & window_prefs{ prefs["window"] };
		ML_assert(window->open(
			window_prefs["title"],
			window_prefs["video"],
			window_prefs["context"],
			window_prefs["hints"]));
		if (install_callbacks)
		{
			static struct ML_NODISCARD {
				event_bus * bus;
				ML_NODISCARD auto operator->() const noexcept { return ML_check(bus); }
			} helper; helper.bus = bus;

			window->set_char_callback([](auto w, auto ... x) { helper->fire<window_char_event>(x...); });
			window->set_char_mods_callback([](auto w, auto ... x) { helper->fire<window_char_mods_event>(x...); });
			window->set_close_callback([](auto w, auto ... x) { helper->fire<window_close_event>(x...); });
			window->set_cursor_enter_callback([](auto w, auto ... x) { helper->fire<window_cursor_enter_event>(x...); });
			window->set_cursor_pos_callback([](auto w, auto ... x) { helper->fire<window_cursor_pos_event>(x...); });
			window->set_content_scale_callback([](auto w, auto ... x) { helper->fire<window_content_scale_event>(x...); });
			window->set_drop_callback([](auto w, auto ... x) { helper->fire<window_drop_event>(x...); });
			window->set_error_callback([](auto ... x) { helper->fire<window_error_event>(x...); });
			window->set_focus_callback([](auto w, auto ... x) { helper->fire<window_focus_event>(x...); });
			window->set_framebuffer_resize_callback([](auto w, auto ... x) { helper->fire<window_framebuffer_resize_event>(x...); });
			window->set_iconify_callback([](auto w, auto ... x) { helper->fire<window_iconify_event>(x...); });
			window->set_key_callback([](auto w, auto ... x) { helper->fire<window_key_event>(x...); });
			window->set_maximize_callback([](auto w, auto ... x) { helper->fire<window_maximize_event>(x...); });
			window->set_mouse_callback([](auto w, auto ... x) { helper->fire<window_mouse_event>(x...); });
			window->set_position_callback([](auto w, auto ... x) { helper->fire<window_position_event>(x...); });
			window->set_refresh_callback([](auto w, auto ... x) { helper->fire<window_refresh_event>(x...); });
			window->set_resize_callback([](auto w, auto ... x) { helper->fire<window_resize_event>(x...); });
			window->set_scroll_callback([](auto w, auto ... x) { helper->fire<window_scroll_event>(x...); });
		}

		ML_assert(ImGui_Startup(window, install_callbacks));
		if (runtime_prefs.contains("dockspace")) {
			auto & dock_prefs{ runtime_prefs["dockspace"] };
			dock_prefs["alpha"].get_to(docker->Alpha);
			dock_prefs["border"].get_to(docker->Border);
			dock_prefs["padding"].get_to(docker->Padding);
			dock_prefs["rounding"].get_to(docker->Rounding);
			dock_prefs["size"].get_to(docker->Size);
		}

		bus->fire<app_enter_event>();
	});
	
	mainloop->set_exit_callback([&]()
	{
		bus->fire<app_exit_event>();

		ImGui_Shutdown(window, imgui);
	});
	
	mainloop->set_idle_callback([&]()
	{
		main_window::poll_events();

		bus->fire<app_idle_event>();

		ImGui_DoFrame(window, window->get_imgui(), [&]() noexcept
		{
			ImGuiExt_ScopeID(app.get());

			(*docker)(imgui->Viewports[0], [&]() noexcept
			{
				bus->fire<imgui_dockspace_event>(docker);
			});

			bus->fire<imgui_render_event>(imgui);
		});

		if (window->has_hints(window_hints_doublebuffer))
		{
			main_window::swap_buffers(window->get_handle());
		}
	});

	// dummy
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
		} break;

		case app_idle_event::ID: {
			for (auto & fb : m_fb) { fb->resize(m_resolution); }
			window->render(
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
					ImGuiExt::HelpMarker("viewport"); ImGui::Separator();

					ImGui::ColorEdit4("clear color", m_clear_color,
						ImGuiColorEditFlags_NoInputs |
						ImGuiColorEditFlags_NoLabel);
					ImGui::Separator();
					ImGui::EndMenuBar();
				}

				ImGui::Image(
					m_fb[0]->get_color_attachments()[0]->get_handle(),
					m_resolution = ImGui::GetContentRegionAvail(),
					{ 0, 1 },
					{ 1, 0 },
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