/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/engine/Application.hpp>
#include <modus_core/engine/PluginManager.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/scene/Components.hpp>
#include <modus_core/graphics/Viewport.hpp>

#include <modus_core/engine/EngineEvents.hpp>
#include <modus_core/window/WindowEvents.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	struct ML_PLUGIN_API sandbox final : plugin
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// panels
		enum
		{
			viewport_panel,
			terminal_panel,
			settings_panel,
			MAX_PANEL
		};
		ImGuiExt::Panel m_panels[MAX_PANEL]
		{
			{ "viewport", true, ImGuiWindowFlags_MenuBar },
			{ "terminal", false, ImGuiWindowFlags_MenuBar },
			{ "settings", true, ImGuiWindowFlags_None },
		};

		// terminal
		stream_sniper m_cout{ &std::cout };
		ImGuiExt::Terminal m_term{};

		// rendering
		bool m_view_dirty{};
		camera m_camera{};
		viewport m_viewport{};

		// ImGuizmo
		int32 gizmoCount{ 1 };
		int32 lastUsing{ 0 };
		mat4 objectMatrix[4] =
		{
			{
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				0.f, 0.f, 0.f, 1.f
			},
			{
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				2.f, 0.f, 0.f, 1.f
			},
			{
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				2.f, 0.f, 2.f, 1.f
			},
			{
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				0.f, 0.f, 2.f, 1.f
			}
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~sandbox() noexcept final {}

		sandbox(plugin_manager * manager, void * userptr) : plugin{ manager, userptr }
		{
			subscribe<
				app_enter_event,
				app_exit_event,
				app_idle_event,
				imgui_dockspace_event,
				imgui_menubar_event,
				imgui_render_event,
				window_cursor_pos_event,
				window_key_event,
				window_mouse_event
			>();
		}

		void on_event(event const & value) final
		{
			switch (value)
			{
			case app_enter_event		::ID: return on_app_enter((app_enter_event &&)value);
			case app_exit_event			::ID: return on_app_exit((app_exit_event &&)value);
			case app_idle_event			::ID: return on_app_idle((app_idle_event &&)value);
			case imgui_dockspace_event	::ID: return on_imgui_dockspace((imgui_dockspace_event &&)value);
			case imgui_menubar_event	::ID: return on_imgui_menubar((imgui_menubar_event &&)value);
			case imgui_render_event		::ID: return on_imgui_render((imgui_render_event &&)value);
			case window_cursor_pos_event::ID: return on_window_cursor_pos((window_cursor_pos_event &&)value);
			case window_key_event		::ID: return on_window_key((window_key_event &&)value);
			case window_mouse_event		::ID: return on_window_mouse((window_mouse_event &&)value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_app_enter(app_enter_event const & ev)
		{
			if (bitmap const i{ get_app()->path_to("resource/modus_launcher.png"), false })
			{
				get_app()->get_main_window()->set_icons(
					i.width(),
					i.height(),
					i.data());
			}

			m_viewport.set_clear_color({ 0.223f, 0.f, 0.46f, 1.f });
			m_viewport.set_clear_flags(gfx::clear_color | gfx::clear_depth);
			m_viewport.set_resolution({ 1280, 720 });
			m_viewport.new_framebuffer();
		}

		void on_app_exit(app_exit_event const & ev)
		{
		}

		void on_app_idle(app_idle_event const & ev)
		{
			ML_defer(&) { m_term.Output.Dump(m_cout.sstr()); };

			auto const dt{ get_app()->get_main_loop()->delta_time() };

			if (m_camera.is_perspective)
			{
				perspective(
					m_camera.fov,
					m_viewport.get_aspect(),
					0.01f,
					100.f,
					m_camera.proj);
			}
			else
			{
				auto view_height{ m_camera.view_width * m_viewport.get_aspect_inverse() };
				orthographic(
					-m_camera.view_width,
					m_camera.view_width,
					-view_height,
					view_height,
					1000.f,
					-1000.f,
					m_camera.proj);
			}

			if (static bool once{}; m_view_dirty || (!once && (once = true)))
			{
				vec3 eye{
					cosf(m_camera.x_angle) * cosf(m_camera.y_angle) * m_camera.distance,
					sinf(m_camera.y_angle) * m_camera.distance,
					sinf(m_camera.x_angle) * cosf(m_camera.y_angle) * m_camera.distance
				};
				vec3 at{ 0.f, 0.f, 0.f };
				vec3 up{ 0.f, 1.f, 0.f };
				look_at(eye, at, up, m_camera.view);
			}

			m_viewport.set_clear_color
			(
				util::rotate_hue(m_viewport.get_clear_color(), dt * 10)
			);

			for (auto const & fb : m_viewport.get_framebuffers())
			{
				fb->resize(m_viewport.get_resolution());
			}

			gfx::draw_list draw_list
			{
				gfx::command::bind_framebuffer(m_viewport.get_framebuffer(0)),
				gfx::command::set_clear_color(m_viewport.get_clear_color()),
				gfx::command::clear(m_viewport.get_clear_flags()),
				[&](gfx::render_context * ctx) noexcept { /* custom rendering */ },
				gfx::command::bind_framebuffer(nullptr)
			};
			get_app()->get_main_window()->get_render_context()->execute(draw_list);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_imgui_dockspace(imgui_dockspace_event const & ev)
		{
			ImGui::DockBuilderDockWindow(m_panels[viewport_panel].Title, ev->GetID());
			ImGui::DockBuilderDockWindow(m_panels[settings_panel].Title, ev->GetID());
		}

		void on_imgui_menubar(imgui_menubar_event const & ev)
		{
			if (ImGui::BeginMenu("file")) {
				if (ImGui::MenuItem("new", "ctrl+n")) {}
				if (ImGui::MenuItem("open", "ctrl+o")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("close")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("save", "ctrl+s")) {}
				if (ImGui::MenuItem("save as", "ctrl+shift+s")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("quit", "alt+f4")) { get_app()->quit(); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("edit")) {
				if (ImGui::MenuItem("undo", "ctrl+z")) {}
				if (ImGui::MenuItem("redo", "ctrl+y")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("cut", "ctrl+x")) {}
				if (ImGui::MenuItem("copy", "ctrl+c")) {}
				if (ImGui::MenuItem("paste", "ctrl+v")) {}
				if (ImGui::MenuItem("duplicate", "ctrl+d")) {}
				if (ImGui::MenuItem("delete", "del")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("select all", "ctrl+a")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("view")) {
				ImGuiExt::MenuItem(m_panels + settings_panel);
				ImGuiExt::MenuItem(m_panels + terminal_panel);
				ImGuiExt::MenuItem(m_panels + viewport_panel);
				ImGui::EndMenu();
			}
		}

		void on_imgui_render(imgui_render_event const & ev)
		{
			ImGuizmo::SetOrthographic(!m_camera.is_perspective);

			draw_viewport(ev); // VIEWPORT
			draw_terminal(ev); // TERMINAL
			draw_settings(ev); // SETTINGS
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_terminal(imgui_render_event const & ev)
		{
			// setup
			static ML_block(&)
			{
				// session
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
			};

			if (m_panels[terminal_panel].IsOpen) {
				static auto const winsize{ (vec2)get_app()->get_main_window()->get_size() };
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
			if (!m_panels[terminal_panel]([&](auto) noexcept
			{
				ImGui::PopStyleVar(1);
	
				// menubar
				if (ImGui::BeginMenuBar()) {
	
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
				
				// draw
				ImGuiExt::ChildWindow("##output", { 0, -ImGui::GetFrameHeightWithSpacing() }, false, ImGuiWindowFlags_HorizontalScrollbar, [&]()
				{
					m_term.Output.Draw();
				});
				ImGuiExt::ChildWindow("##input", {}, false, ImGuiWindowFlags_NoScrollbar, [&]()
				{
					m_term.DrawPrefix(); ImGui::SameLine();
					m_term.DrawInput();
				});
			}))
			{
				ImGui::PopStyleVar(1);
			}
			
		}

		void draw_viewport(imgui_render_event const & ev)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel]([&](auto) noexcept
			{
				ImGui::PopStyleVar(1);

				ImGuizmo::SetDrawlist();

				if (ImGui::BeginMenuBar()) {
					auto const fps{ get_app()->get_fps()->value };
					ImGui::SetNextItemWidth(250.f);
					ImGui::TextDisabled("%.3f ms/frame ( %.1f fps )", 1000.f / fps, fps);
					ImGui::Separator();
					auto cc{ m_viewport.get_clear_color() };
					if (ImGui::ColorEdit4("clear color", cc, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
					{
						m_viewport.set_clear_color(cc);
					}
					ImGui::Separator();
					ImGui::EndMenuBar();
				}

				auto view_pos{ (vec2)ImGui::GetCursorPos() };
				auto view_size{ (vec2)ImGui::GetContentRegionAvail() };
				
				m_viewport.set_position(view_pos);
				m_viewport.set_resolution(view_size);
				ImGui::Image(
					m_viewport.get_framebuffer(0)->get_color_attachment(0)->get_handle(),
					view_size,
					{ 0, 1 }, { 1, 0 },
					colors::white,
					colors::clear);

				ImGui::SetCursorPos(view_pos);
				ImGui::Text("VX: %f VY: %f", view_pos[0], view_pos[1]);
				ImGui::Text("CX: %f CY: %f", ev->IO.MousePos.x, ev->IO.MousePos.y);
				ImGuizmo::DrawGrid(m_camera.view, m_camera.proj, mat4::identity(), 100.f);
				ImGuizmo::DrawCubes(m_camera.view, m_camera.proj, &objectMatrix[0][0], gizmoCount);
				for (int32 matId = 0; matId < gizmoCount; matId++)
				{
					ImGuizmo::SetID(matId);
					ImGuiExt::EditTransform(
						m_camera.view,
						m_camera.proj,
						objectMatrix[matId],
						(float_rect)m_viewport.get_bounds(),
						false);
					if (ImGuizmo::IsUsing()) {
						lastUsing = matId;
					}
				}
			}))
			{
				ImGui::PopStyleVar(1);
			}
		}

		void draw_settings(imgui_render_event const & ev)
		{
			if (m_panels[settings_panel].IsOpen) {
				ImGui::SetNextWindowPos({ 64, 64 }, ImGuiCond_Once);
				ImGui::SetNextWindowSize({ 320, 340 }, ImGuiCond_Once);
			}
			m_panels[settings_panel]([&](auto) noexcept
			{
				ImGui::Text("Camera");
				if (ImGui::RadioButton("Perspective", m_camera.is_perspective)) {
					m_camera.is_perspective = true;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Orthographic", !m_camera.is_perspective)) {
					m_camera.is_perspective = false;
				}
				if (m_camera.is_perspective) {
					ImGui::SliderFloat("Fov", &m_camera.fov, 20.f, 110.f);
				}
				else {
					ImGui::SliderFloat("Ortho width", &m_camera.view_width, 1, 20);
				}
				m_view_dirty |= ImGui::SliderFloat("Distance", &m_camera.distance, 1.f, 10.f);
				ImGui::SliderInt("Gizmo count", &gizmoCount, 1, 4);

				ImGui::Separator();
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_window_cursor_pos(window_cursor_pos_event const & ev)
		{
		}

		void on_window_key(window_key_event const & ev)
		{
		}

		void on_window_mouse(window_mouse_event const & ev)
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern "C"
{
	ML_PLUGIN_API ml::plugin * ml_plugin_create(ml::plugin_manager * manager, void * userptr)
	{
		return manager->allocate_plugin<ml::sandbox>(userptr);
	}

	ML_PLUGIN_API void ml_plugin_destroy(ml::plugin_manager * manager, ml::plugin * ptr)
	{
		manager->deallocate_plugin<ml::sandbox>(ptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */