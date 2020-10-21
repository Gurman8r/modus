/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/client/ClientRuntime.hpp>
#include <modus_core/client/ClientEvents.hpp>
#include <modus_core/client/ClientDatabase.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/graphics/Font.hpp>
#include <modus_core/graphics/Mesh.hpp>
#include <modus_core/graphics/Shader.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/client/ImGuiExt.hpp>
#include <modus_core/scene/SceneManager.hpp>
#include <modus_core/scene/Viewport.hpp>
#include <modus_core/window/WindowEvents.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	struct ML_PLUGIN_API sandbox final : plugin
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// panel ids
		enum : size_t
		{
			imgui_about_panel,
			imgui_demo_panel,
			imgui_metrics_panel,
			imgui_style_panel,

			database_panel,
			console_panel,
			settings_panel,
			viewport_panel,

			MAX_PANEL
		};

		// panels
		ImGuiExt::Panel m_panels[MAX_PANEL]
		{
			{ "About Dear ImGui" },
			{ "Dear ImGui Demo" },
			{ "Dear ImGui Metrics" },
			{ "Style Editor" },
			
			{ "database"	, 0, ImGuiWindowFlags_None },
			{ "console"		, 1, ImGuiWindowFlags_None },
			{ "settings"	, 0, ImGuiWindowFlags_MenuBar },
			{ "viewport"	, 1, ImGuiWindowFlags_MenuBar },
		};

		// console
		ImGuiExt::CommandLine	m_console	{};
		basic_stream_sniper<>	m_cout		{ &std::cout };

		// rendering
		vec2 m_resolution{ 1280, 720 };
		color m_clear_color{ 0.4f, 0.f, 1.f, 1.f };
		pmr::vector<shared<gfx::framebuffer>> m_fb{};

		// database
		db_var< ds::hashmap<pmr::string, bitmap> > m_images{ get_db(), "images" };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~sandbox() noexcept override {}

		sandbox(plugin_manager * manager, void * user) noexcept : plugin{ manager, user }
		{
			subscribe<client_enter_event>();
			subscribe<client_exit_event>();
			subscribe<client_idle_event>();
			subscribe<client_dock_event>();
			subscribe<client_menu_event>();
			subscribe<client_gui_event>();
		}

		void on_event(event && value) override
		{
			switch (value)
			{
			case client_enter_event	::ID: return on_client_enter((client_enter_event &&)value);
			case client_exit_event	::ID: return on_client_exit((client_exit_event &&)value);
			case client_idle_event	::ID: return on_client_idle((client_idle_event &&)value);
			case client_dock_event	::ID: return on_client_dock((client_dock_event &&)value);
			case client_menu_event	::ID: return on_client_menu((client_menu_event &&)value);
			case client_gui_event	::ID: return on_client_gui((client_gui_event &&)value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_client_enter(client_enter_event && ev)
		{
			if (bitmap & i = m_images["icon"] = { get_io()->path2("resource/icon.png") })
			{
				get_window()->set_icons(i.width(), i.height(), 1, i.data());
			}

			m_fb.push_back(gfx::framebuffer::create({ m_resolution }));
		}

		void on_client_exit(client_exit_event && ev)
		{
		}

		void on_client_idle(client_idle_event && ev)
		{
			m_console.Output.Dump(m_cout.sstr());

			for (auto & fb : m_fb) { fb->resize(m_resolution); }

			get_window()->execute(
				gfx::command::bind_framebuffer(m_fb[0]),
				gfx::command::set_clear_color(m_clear_color),
				gfx::command::clear(gfx::clear_color | gfx::clear_depth),
				gfx::command([&](gfx::render_context * ctx) noexcept
				{
					// custom rendering...
				}),
				gfx::command::bind_framebuffer(nullptr));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_client_dock(client_dock_event && ev)
		{
			if (auto const root{ ImGui::GetID(ev->get_dock_title().c_str()) }
			; !ImGui::DockBuilderGetNode(root))
			{
				ImGui::DockBuilderRemoveNode(root);
				ImGui::DockBuilderAddNode(root, ev->get_dock_flags());
				ImGui::DockBuilderDockWindow(m_panels[viewport_panel].title, root);
				ImGui::DockBuilderFinish(root);
			}
		}

		void on_client_menu(client_menu_event && ev)
		{
			// FILE
			if (ImGui::BeginMenu("file")) {
				if (ImGui::MenuItem("quit", "alt+f4")) {
					get_window()->set_should_close(true);
				}
				ImGui::EndMenu();
			}

			// TOOLS
			if (ImGui::BeginMenu("tools")) {
				ImGuiExt::MenuItem(m_panels[console_panel]);
				ImGuiExt::MenuItem(m_panels[database_panel]);
				ImGuiExt::MenuItem(m_panels[settings_panel]);
				ImGuiExt::MenuItem(m_panels[viewport_panel]);
				ImGui::EndMenu();
			}

			// HELP
			if (ImGui::BeginMenu("help")) {
				ImGuiExt::MenuItem(m_panels[imgui_demo_panel]);
				ImGuiExt::MenuItem(m_panels[imgui_metrics_panel]);
				ImGuiExt::MenuItem(m_panels[imgui_style_panel]);
				ImGuiExt::MenuItem(m_panels[imgui_about_panel]);
				ImGui::EndMenu();
			}
		}

		void on_client_gui(client_gui_event && ev)
		{
			// IMGUI
			if (m_panels[imgui_about_panel].open) {
				ImGui::ShowAboutWindow(&m_panels[imgui_about_panel].open);
			}
			if (m_panels[imgui_demo_panel].open) {
				ImGui::ShowDemoWindow(&m_panels[imgui_demo_panel].open);
			}
			if (m_panels[imgui_metrics_panel].open) {
				ImGui::ShowMetricsWindow(&m_panels[imgui_metrics_panel].open);
			}
			m_panels[imgui_style_panel](
				&ImGui::ShowStyleEditor, &ImGui::GetStyle()
			);

			// SANDBOX
			draw_console_panel	(); // CONSOLE
			draw_database_panel	(); // DATABASE
			draw_settings_panel	(); // SETTINGS
			draw_viewport_panel	(); // VIEWPORT
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// CONSOLE
		void draw_console_panel()
		{
			if (m_panels[console_panel].open) {
				auto const winsize{ (vec2)get_window()->get_size() };
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			m_panels[console_panel]([&]() noexcept
			{
				if (m_console.Draw(); !m_console.Commands.empty()) { return; }

				// help
				m_console.Commands.push_back({ "help", {}, [&](auto && line) {
					for (auto const & cmd : m_console.Commands) {
						std::cout << cmd.name << "\n";
					}
				} });

				// clear
				m_console.Commands.push_back({ "clear", {}, [&](auto && line) {
					m_console.Output.Clear();
				} });

				// exit
				m_console.Commands.push_back({ "exit", {}, [&](auto && line) {
					get_window()->set_should_close(true);
				} });

				// echo
				m_console.Commands.push_back({ "echo", {}, [&](auto && line) {
					std::cout << line << "\n";
				} });

				// py
				m_console.Commands.push_back({ "py", {}, [&](auto && line) {
					if (line.empty()) {
						if (m_console.Domain.empty()) {
							m_console.Domain = "py";
							static ML_scope() { std::cout << "# type '\\' to stop using python\n"; };
							return;
						}
					}
					else if (line == "\\") {
						m_console.Domain.clear();
						return;
					}
					PyRun_SimpleString(line.c_str());
				} });
			});
		}

		// DATABASE
		void draw_database_panel()
		{
			if (m_panels[database_panel].open) {
				auto const winsize{ (vec2)get_window()->get_size() };
				ImGui::SetNextWindowSize({ 960, 329 }, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			m_panels[database_panel]([&]() noexcept
			{
				ML_defer() { ImGui::EndTabBar(); };
				if (!ImGui::BeginTabBar("tabs")) { return; }

				// plugins
				if (ImGui::BeginTabItem("plugins")) {
					ImGui::Columns(3);
					ImGui::TextDisabled("name"); ImGui::NextColumn();
					ImGui::TextDisabled("path"); ImGui::NextColumn();
					ImGui::TextDisabled("ID"); ImGui::NextColumn();
					ImGui::Separator();
					auto const & details{ get_manager()->get_storage().get<plugin_details>() };
					for (auto const & e : details)
					{
						ImGui::Text(e.name.c_str()); ImGui::NextColumn();
						ImGui::Text(e.path.c_str()); ImGui::NextColumn();
						ImGui::Text("%u", e.id); ImGui::NextColumn();
					}
					ImGui::Columns(1);
					ImGui::Separator();
					ImGui::EndTabItem();
				}
			});
		}

		// SETTINGS
		void draw_settings_panel()
		{
			if (m_panels[settings_panel].open) {
				auto const winsize{ (vec2)get_window()->get_size() };
				ImGui::SetNextWindowSize({ 320, 512 }, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			m_panels[settings_panel]([&]() noexcept
			{
				if (ImGui::BeginMenuBar()) {
					ImGuiExt::HelpMarker("settings");
					ImGui::Separator();
					ImGui::EndMenuBar();
				}
			});
		}

		// VIEWPORT
		void draw_viewport_panel()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel]([&]() noexcept
			{
				ImGui::PopStyleVar(1);

				if (ImGui::BeginMenuBar()) {
					ImGuiExt::HelpMarker("viewport");
					ImGui::Separator();
					
					ImGui::ColorEdit4("clear color", m_clear_color,
						ImGuiColorEditFlags_NoInputs |
						ImGuiColorEditFlags_NoLabel);
					ImGui::Separator();

					auto const fps{ get_io()->fps };
					ImGui::TextDisabled("%.3f ms/frame ( %.1f fps )", 1000.f / fps, fps);
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
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern "C"
{
	ML_PLUGIN_API ml::plugin * ml_plugin_attach(ml::plugin_manager * manager, void * user)
	{
		return manager->get_memory()->new_object<ml::sandbox>(manager, user);
	}

	ML_PLUGIN_API void ml_plugin_detach(ml::plugin_manager * manager, ml::plugin * ptr)
	{
		manager->get_memory()->delete_object((ml::sandbox *)ptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */