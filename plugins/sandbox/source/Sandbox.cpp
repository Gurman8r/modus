/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/engine/Application.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/scene/Components.hpp>
#include <modus_core/engine/PluginManager.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	struct ML_PLUGIN_API sandbox final : plugin
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// panels
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

		// terminal
		basic_stream_sniper<> m_cout{ &std::cout };
		ImGuiExt::Terminal m_term{};

		// rendering
		vec2 m_resolution{ 1280, 720 };
		color m_clear_color{ 0.223f, 0.f, 0.46f, 1.f };
		ds::list<ds::ref<gfx::framebuffer>> m_fb{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~sandbox() noexcept final {}

		sandbox(plugin_manager * manager, void * userptr) : plugin{ manager, userptr }
		{
			subscribe<
				app_enter_event,
				app_exit_event,
				app_idle_event,
				imgui_dockspace_event,
				imgui_render_event
			>();
		}

		void on_event(event const & value) final
		{
			switch (value)
			{
			case app_enter_event::ID: {
				if (bitmap i{ get_app()->path_to("resource/modus_launcher.png"), false })
				{
					get_app()->get_main_window()->set_icons(i.width(), i.height(), i.data());
				}
				m_fb.push_back(gfx::make_framebuffer((vec2i)m_resolution));
			} break;
	
			case app_exit_event::ID: {
				m_cout.update(nullptr);
			} break;
	
			case app_idle_event::ID: {
				m_term.Output.Dump(m_cout.sstr());
				for (auto & fb : m_fb) { fb->resize(m_resolution); }
				get_app()->get_render_context()->execute(
					gfx::command::bind_framebuffer(m_fb[0]),
					gfx::command::set_clear_color(m_clear_color),
					gfx::command::clear(gfx::clear_color | gfx::clear_depth),
					gfx::command([&](gfx::render_context * ctx) noexcept { /* custom rendering */ }),
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

						if (ImGui::MenuItem("new")) {
						}
						if (ImGui::MenuItem("open")) {
						}
						if (ImGui::MenuItem("close")) {
						}
						ImGui::Separator();
						if (ImGui::MenuItem("save")) {
						}
						if (ImGui::MenuItem("save as...")) {
						}
						ImGui::Separator();
						if (ImGui::MenuItem("quit", "alt+f4")) {
							get_app()->quit();
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("tools")) {
						ImGuiExt::MenuItem(m_panels + terminal_panel);
						ImGuiExt::MenuItem(m_panels + viewport_panel);
						ImGui::EndMenu();
					}
					ImGui::EndMainMenuBar();
				}
	
				show_terminal(); // TERMINAL

				show_viewport(); // VIEWPORT
	
			} break;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void show_terminal()
		{
			if (m_panels[terminal_panel].IsOpen) {
				auto const winsize{ (vec2)get_app()->get_main_window()->get_size() };
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
			if (!m_panels[terminal_panel]([&]() noexcept
			{
				ImGui::PopStyleVar(1);
	
				// menubar
				if (ImGui::BeginMenuBar()) {
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
				ImGuiExt::ChildWindow("##output", { 0, -ImGui::GetFrameHeightWithSpacing() }, false, ImGuiWindowFlags_HorizontalScrollbar, [&]()
				{
					m_term.Output.Draw();
				});
				ImGuiExt::ChildWindow("##input", {}, false, ImGuiWindowFlags_NoScrollbar, [&]()
				{
					m_term.DrawPrefix(); ImGui::SameLine();
					m_term.DrawInput();
				});
	
				// setup
				if (!m_term.Commands.empty()) { return; }
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
					get_app()->quit();
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
			}))
			{
				ImGui::PopStyleVar(1);
			}
		}

		void show_viewport()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel]([&]() noexcept
			{
				ImGui::PopStyleVar(1);

				if (ImGui::BeginMenuBar()) {
					ImGuiExt::HelpMarker("viewport");
					ImGui::Separator();
					ImGui::ColorEdit4("clear color", m_clear_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					ImGui::Separator();
					auto const fps{ get_app()->get_frame_rate() };
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
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern "C"
{
	ML_PLUGIN_API ml::plugin * ml_plugin_install(ml::plugin_manager * manager, void * userptr)
	{
		return manager->get_memory()->new_object<ml::sandbox>(manager, userptr);
	}

	ML_PLUGIN_API void ml_plugin_uninstall(ml::plugin_manager * manager, ml::plugin * ptr)
	{
		return manager->get_memory()->delete_object((ml::sandbox *)ptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */