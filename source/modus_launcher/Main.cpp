#include <modus_core/engine/Application.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/scene/Components.hpp>

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

json load_settings(fs::path const & path = SETTINGS_PATH)
{
	std::ifstream f{ path }; ML_defer(&f) { f.close(); };

	return f ? json::parse(f) : default_settings;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t main(int32_t argc, char * argv[])
{
	static memory_manager memory{};

	auto app{ make_scope<application>(argc, argv) };
	app->set_app_name(ML__name);
	app->set_app_version(ML__version);
	app->set_attributes(load_settings());
	app->set_library_paths(app->attr("paths"));

	// demo
	ds::list<ds::ref<gfx::framebuffer>> m_fb{};
	color m_clear_color{ 0.223f, 0.f, 0.46f, 1.f };
	vec2 m_resolution{ 1280, 720 };
	basic_stream_sniper<> m_cout{ &std::cout };
	ImGuiExt::Terminal m_term{};
	enum
	{
		terminal_panel,
		viewport_panel,
		MAX_PANEL
	};
	ImGuiExt::Panel m_panels[MAX_PANEL]
	{
		{ "terminal", true, ImGuiWindowFlags_MenuBar },
		{ "viewport", true, ImGuiWindowFlags_MenuBar },
	};
	app->get_bus()->new_dummy<
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
			if (bitmap i; i.load_from_file(app->path_to("resource/modus_launcher.png"), false))
			{
				app->get_main_window()->set_icons(i.width(), i.height(), i.data());
			}
			m_fb.push_back(gfx::make_framebuffer((vec2i)m_resolution));
		} break;
	
		case app_exit_event::ID: {
			m_cout.update(nullptr);
		} break;
	
		case app_idle_event::ID: {
			m_term.Output.Dump(m_cout.sstr());
			for (auto & fb : m_fb) { fb->resize(m_resolution); }
			app->get_main_window()->render_commands(
				gfx::command::bind_framebuffer(m_fb[0]),
				gfx::command::set_clear_color(m_clear_color),
				gfx::command::clear(gfx::clear_color | gfx::clear_depth),
				gfx::command([&](gfx::render_context * ctx) noexcept { /* custom */ }),
				gfx::command::bind_framebuffer(nullptr));
		} break;
	
		case imgui_dockspace_event::ID: {
			auto && ev{ (imgui_dockspace_event &&)value };
			ImGui::DockBuilderDockWindow(m_panels[viewport_panel].Title, ev->GetID());
		} break;
	
		case imgui_render_event::ID: {
	
			// MAIN MENU BAR
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("file")) {
					if (ImGui::MenuItem("quit", "alt+f4")) { app->quit(); }
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("tools")) {
					ImGuiExt::MenuItem(m_panels + terminal_panel);
					ImGuiExt::MenuItem(m_panels + viewport_panel);
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
	
			// VIEWPORT
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel]([&]() noexcept
			{
				ImGui::PopStyleVar(1);
	
				if (ImGui::BeginMenuBar()) {
					ImGuiExt::HelpMarker("viewport");
					ImGui::Separator();
					ImGui::ColorEdit4("clear color", m_clear_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					ImGui::Separator();
					auto const fps{ app->get_frame_rate() };
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
	
			// TERMINAL
			if (m_panels[terminal_panel].IsOpen) {
				auto const winsize{ (vec2)app->get_main_window()->get_size() };
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
			if (!m_panels[terminal_panel]([&]() noexcept
			{
				ImGui::PopStyleVar(1);
	
				// menubar
				if (ImGui::BeginMenuBar())
				{
					ImGuiExt::HelpMarker("terminal");
					ImGui::Separator();
	
					// filter
					ImGui::TextDisabled("filter"); ImGui::SameLine();
					m_term.Output.Filter.Draw("##filter", 256);
					ImGui::Separator();
	
					// options
					if (ImGui::BeginMenu("options"))
					{
						ImGui::Checkbox("auto scroll", &m_term.Output.AutoScroll);
						ImGui::Separator();
						m_term.DrawPrefixOptions();
						ImGui::EndMenu();
					}
					ImGui::Separator();
	
					// clear
					if (ImGui::MenuItem("clear")) { m_term.Output.Lines.clear(); }
					ImGui::Separator();
	
					ImGui::EndMenuBar();
				}
				
				// draw terminal
				ImGuiExt::ChildWindow("##output", { 0, -ImGui::GetFrameHeightWithSpacing() }, false, ImGuiWindowFlags_HorizontalScrollbar, [&]() noexcept
				{
					m_term.Output.Draw();
				});
				ImGuiExt::ChildWindow("##input", {}, false, ImGuiWindowFlags_NoScrollbar, [&]() noexcept
				{
					m_term.DrawPrefix(); ImGui::SameLine();
					m_term.DrawInput();
				});
	
				// setup
				if (m_term.Commands.empty())
				{
					m_term.User = "root";
					m_term.Host = "localhost";
					m_term.Path = "~";
					m_term.Mode = "";
	
					// clear
					m_term.AddCommand("clear", {}, [&](auto line) {
						m_term.Output.Lines.clear();
					});
	
					// exit
					m_term.AddCommand("exit", {}, [&](auto line) {
						app->quit();
					});
	
					// help
					m_term.AddCommand("help", {}, [&](auto line) {
						for (auto const & name : m_term.Commands.get<0>()) {
							debug::puts("- {0}", name);
						}
					});
	
					// history
					m_term.AddCommand("history", {}, [&](auto line) {
						for (auto const & str : m_term.History) {
							debug::puts(str);
						}
					});
	
					// python
					m_term.AddCommand("python", {}, [&](auto line) {
						// lock
						if (m_term.Mode.empty() && line.empty()) {
							m_term.Mode = "python"; return;
						}
						// unlock
						else if (m_term.Mode == line && line == "python") {
							m_term.Mode.clear(); return;
						}
						// evaluate
						PyRun_SimpleString(line.c_str());
					});
				}
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