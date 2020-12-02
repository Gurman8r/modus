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
			{ "settings", false, ImGuiWindowFlags_None },
		};

		// terminal
		stream_sniper m_cout{ &std::cout };
		ImGuiExt::Terminal m_term{};

		// rendering
		bool m_cycle_color{ true };
		bool m_view_dirty{};
		camera m_cam{};
		viewport m_view{};

		// ImGuizmo
		int32 m_gizmo_count{ 1 };
		int32 m_last_using{ 0 };
		mat4 m_object_matrix[4] =
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
				get_app()->get_window()->set_icons(
					i.width(),
					i.height(),
					i.data());
			}

			m_cam.set_clear_color({ 0.223f, 0.f, 0.46f, 1.f });
			m_cam.set_clear_flags(gfx::clear_color | gfx::clear_depth);
			m_cam.set_position({ -5.f, 5.f, -5.f });
			m_cam.set_forward({ 1.f, -1.f, 1.f });

			m_view.set_size({ 1280, 720 });
			m_view.new_framebuffer();
		}

		void on_app_exit(app_exit_event const & ev)
		{
		}

		void on_app_idle(app_idle_event const & ev)
		{
			ML_defer(&) { m_term.Output.Dump(m_cout.sstr()); };

			m_cam.recalculate(m_view.get_size());

			for (auto const & fb : m_view.get_framebuffers())
			{
				fb->resize(m_view.get_size());
			}

			if (auto const & cc{ m_cam.get_clear_color() }; m_cycle_color)
			{
				m_cam.set_clear_color(util::rotate_hue(cc, ev.delta_time * 10));
			}

			gfx::draw_list draw_list{};
			draw_list +=
			{
				gfx::command::bind_framebuffer(m_view.get_framebuffer(0)),
				gfx::command::set_clear_color(m_cam.get_clear_color()),
				gfx::command::clear(m_cam.get_clear_flags()),
				[&](gfx::render_context * ctx) noexcept { /* custom rendering */ },
				gfx::command::bind_framebuffer(nullptr)
			};
			get_app()->get_window()->get_render_context()->execute(draw_list);
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
			ImGuizmo::SetOrthographic(false);

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
				static auto const winsize{ (vec2)get_app()->get_window()->get_size() };
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
			if (!m_panels[terminal_panel]([&](auto)
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
			auto const window{ get_app()->get_window() };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel]([&](auto)
			{
				ImGui::PopStyleVar(1);

				// imguizmo
				ImGuizmo::SetDrawlist();

				// settings
				static bool
					grid_enabled{ true },
					cubes_enabled{ true };

				// menubar
				if (ImGui::BeginMenuBar())
				{
					// debug
					if (ImGui::RadioButton("grid", grid_enabled)) { grid_enabled = !grid_enabled; }
					ImGui::SameLine();
					if (ImGui::RadioButton("cubes", cubes_enabled)) { cubes_enabled = !cubes_enabled; }
					ImGui::Separator();

					// camera
					if (ImGui::BeginMenu("camera"))
					{
						if (auto cc{ m_cam.get_clear_color() }
						; ImGui::ColorEdit4("##clear color", cc, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoInputs)) {
							m_cam.set_clear_color(cc);
						} ImGui::SameLine();

						auto cf{ m_cam.get_clear_flags() };
						if (ImGui::CheckboxFlags("color##bit", &cf, gfx::clear_color)) {
							m_cam.set_clear_flags(cf);
						} ImGui::SameLine();
						if (ImGui::CheckboxFlags("depth##bit", &cf, gfx::clear_depth)) {
							m_cam.set_clear_flags(cf);
						} ImGui::SameLine();
						if (ImGui::CheckboxFlags("stencil##bit", &cf, gfx::clear_stencil)) {
							m_cam.set_clear_flags(cf);
						}

						ImGui::Separator();

						if (auto pos{ m_cam.get_position() }
						; ImGui::DragFloat3("position", pos, .1f)) {
							m_cam.set_position(pos);
						}

						if (auto fwd{ m_cam.get_forward() }
						; ImGui::DragFloat3("forward", fwd, .1f)) {
							m_cam.set_forward(fwd);
						}

						if (auto up{ m_cam.get_up() }
						; ImGui::DragFloat3("up", up, .1f)) {
							m_cam.set_up(up);
						}

						if (auto world_up{ m_cam.get_world_up() }
						; ImGui::DragFloat3("world up", world_up, .1f)) {
							m_cam.set_world_up(world_up);
						}

						ImGui::Separator();

						if (auto fov{ m_cam.get_fov() }
						; ImGui::SliderFloat("fov", &fov, 0.f, 180.f)) {
							m_cam.set_fov(fov);
						}

						if (auto clip{ m_cam.get_clip() }
						; ImGui::DragFloat2("clip", clip)) {
							m_cam.set_clip(clip);
						}

						ImGui::Separator();

						if (auto yaw{ m_cam.get_yaw() }
						; ImGui::SliderFloat("yaw", &yaw, 0.f, 360.f)) {
							m_cam.set_yaw(yaw);
						}

						if (auto pitch{ m_cam.get_pitch() }
						; ImGui::SliderFloat("pitch", &pitch, 0.f, 360.f)) {
							m_cam.set_pitch(pitch);
						}

						if (auto roll{ m_cam.get_roll() }
						; ImGui::SliderFloat("roll", &roll, 0.f, 360.f)) {
							m_cam.set_roll(roll);
						}

						if (auto zoom{ m_cam.get_zoom() }
						; ImGui::SliderFloat("zoom", &zoom, FLT_MIN, 100.f)) {
							m_cam.set_zoom(zoom);
						}
						ImGui::Separator();

						ImGui::EndMenu();
					}
					ImGui::Separator();

					ImGui::SetNextItemWidth(200.f);
					ImGui::TextDisabled("X: %.1f Y: %.1f", ev->IO.MousePos.x, ev->IO.MousePos.y);
					ImGui::Separator();

					auto const fps{ ev->IO.Framerate };
					ImGui::SetNextItemWidth(200.f);
					ImGui::TextDisabled("%.3f ms/frame ( %.1f fps )", 1000.f / fps, fps);
					ImGui::Separator();

					ImGui::EndMenuBar();
				}

				// viewport
				auto const
					win_pos	{ (vec2)window->get_position() },
					win_size{ (vec2)window->get_size() },
					img_pos	{ (vec2)ImGui::GetCursorPos() },
					img_size{ (vec2)ImGui::GetContentRegionAvail() };
				m_view.set_position(img_pos);
				m_view.set_size(img_size);

				// image
				ImGui::Image(
					m_view.get_framebuffer(0)->get_color_attachment(0)->get_handle(),
					img_size,
					{ 0, 1 }, { 1, 0 },
					colors::white,
					colors::clear);

				// gizmos
				mat4 const view{ m_cam.get_view() }, proj{ m_cam.get_proj() };

				if (grid_enabled) { ImGuizmo::DrawGrid(view, proj, mat4::identity(), 100.f); }
				
				if (cubes_enabled) { ImGuizmo::DrawCubes(view, proj, &m_object_matrix[0][0], m_gizmo_count); }
				
				for (int32 i = 0; i < m_gizmo_count; ++i)
				{
					ImGuizmo::SetID(i);
					ImGuiExt::EditTransform(
						view,
						proj,
						m_object_matrix[i],
						{ win_pos + img_pos, win_size },
						false);
					if (ImGuizmo::IsUsing()) {
						m_last_using = i;
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
			m_panels[settings_panel]([&](auto)
			{
				

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