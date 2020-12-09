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
			memory_panel,
			settings_panel,
			terminal_panel,
			viewport_panel,
			MAX_PANEL
		};
		ImGuiExt::Panel m_panels[MAX_PANEL]
		{
			{ "memory", true, ImGuiWindowFlags_MenuBar },
			{ "settings", false, ImGuiWindowFlags_None },
			{ "terminal", false, ImGuiWindowFlags_MenuBar },
			{ "viewport", true, ImGuiWindowFlags_MenuBar },
		};

		// terminal
		stream_sniper m_cout{ &std::cout }; // stdout wrapper
		ImGuiExt::Terminal m_term{}; // terminal

		// 
		MemoryEditor m_mem_editor{};

		// rendering
		bool m_cycle_bg{ true }; // cycle background
		viewport m_view{}; // viewport
		camera m_camera{}; // camera

		// scene
		ds::ref<scene> m_active_scene{}; // active scene

		// gizmos
		bool m_enable_grid{ true };
		int32 m_object_count{ 1 };
		int32 m_object_index{};
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
			// icon
			if (bitmap const i{ get_app()->path_to("resource/modus_launcher.png"), false })
			{
				get_app()->get_window()->set_icons(i.width(), i.height(), i.data());
			}

			// viewport
			vec2 const resolution{ 1280, 720 };
			m_view.set_resolution(resolution);
			m_view.set_framebuffer(gfx::make_framebuffer(resolution));

			// camera
			m_camera.set_clear_flags(gfx::clear_flags_color | gfx::clear_flags_depth);
			m_camera.set_background({ 0.223f, 0.f, 0.46f, 1.f });
			m_camera.set_orthographic(false);
			m_camera.set_eye({ 5.f, 3.f, 5.f });
			m_camera.set_target({ 0.f, 0.f, 0.f });

			// scene
			m_active_scene = make_ref<scene>(get_bus());
			get_app()->set_active_scene(m_active_scene);
		}

		void on_app_exit(app_exit_event const & ev)
		{
			debug::success("goodbye!");
		}

		void on_app_idle(app_idle_event const & ev)
		{
			ML_defer(&) { m_term.Output.Dump(m_cout.sstr()); };

			auto const main_window{ get_app()->get_window() };

			m_view.recalculate();

			m_camera.recalculate(m_view.get_resolution());

			if (m_cycle_bg)
			{
				m_camera.set_background(util::rotate_hue
				(
					m_camera.get_background(), ev.delta_time * 10
				));
			}

			gfx::draw_list draw_list{};
			draw_list +=
			{
				gfx::command::bind_framebuffer(m_view.get_framebuffer()),
				gfx::command::set_clear_color(m_camera.get_background()),
				gfx::command::clear(m_camera.get_clear_flags()),
				[&](gfx::render_context * ctx) noexcept { /* custom rendering */ },
				gfx::command::bind_framebuffer(0)
			};
			main_window->get_render_context()->execute(draw_list);
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
				ImGuiExt::MenuItem(m_panels + memory_panel);
				ImGuiExt::MenuItem(m_panels + settings_panel);
				ImGuiExt::MenuItem(m_panels + terminal_panel);
				ImGuiExt::MenuItem(m_panels + viewport_panel);
				ImGui::EndMenu();
			}
		}

		void on_imgui_render(imgui_render_event const & ev)
		{
			draw_memory(ev);	// MEMORY
			draw_settings(ev);	// SETTINGS
			draw_terminal(ev);	// TERMINAL
			draw_viewport(ev);	// VIEWPORT
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_memory(imgui_render_event const & ev)
		{
			memory_manager * const memory{ get_global<memory_manager>() };
			passthrough_resource * const view{ memory->get_resource() };
			memory_manager::record_storage const & records{ memory->get_storage() };

			static ML_block(&) // setup
			{
				m_mem_editor.Open				= true;
				m_mem_editor.ReadOnly			= true;
				m_mem_editor.Cols				= 32;
				m_mem_editor.OptShowOptions		= true;
				m_mem_editor.OptShowDataPreview	= true;
				m_mem_editor.OptShowHexII		= false;
				m_mem_editor.OptShowAscii		= true;
				m_mem_editor.OptGreyOutZeroes	= true;
				m_mem_editor.OptUpperCaseHex	= true;
				m_mem_editor.OptMidColsCount	= 8;
				m_mem_editor.OptAddrDigitsCount	= 0;
				m_mem_editor.HighlightColor		= IM_COL32(0, 255, 255, 50);
				m_mem_editor.ReadFn				= nullptr;
				m_mem_editor.WriteFn			= nullptr;
				m_mem_editor.HighlightFn		= nullptr;
			};
			if (m_panels[memory_panel].IsOpen) {
				static auto const winsize{ (vec2)get_app()->get_window()->get_size() };
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			m_panels[memory_panel]([&](auto)
			{
				// menubar
				if (ImGui::BeginMenuBar())
				{
					// read only
					ImGui::Checkbox("read only", &m_mem_editor.ReadOnly);
					ImGui::Separator();

					// progress
					char progress[32] = ""; std::sprintf(progress,
						"%u / %u (%.2f%%)",
						(uint32_t)view->used(),
						(uint32_t)view->capacity(),
						view->percentage()
					);
					ImGui::ProgressBar(view->fraction(), { 256.f, 0.f }, progress);
					ImGuiExt::TooltipEx([&]() noexcept
					{
						ImGui::Text("allocations: %u", view->count());
						ImGui::Text("total:       %u", view->capacity());
						ImGui::Text("in use:      %u", view->used());
						ImGui::Text("available:   %u", view->free());
					});
					ImGui::Separator();

					ImGui::EndMenuBar();
				}

				// contents
				m_mem_editor.DrawContents(view->data(), view->capacity(), view->base());
			});
		}

		void draw_settings(imgui_render_event const & ev)
		{
			if (m_panels[settings_panel].IsOpen) {
				ImGui::SetNextWindowPos({ 64, 64 }, ImGuiCond_Once);
				ImGui::SetNextWindowSize({ 320, 340 }, ImGuiCond_Once);
			}
			if (!m_panels[settings_panel]([&](auto)
			{
			}))
			{
			}
		}

		void draw_terminal(imgui_render_event const & ev)
		{
			static ML_block(&) // setup
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
			static ImGuiExt::TransformEditor xedit{};

			auto const main_window{ get_app()->get_window() };

			ImGuizmo::SetOrthographic(m_camera.is_orthographic());

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel]([&](auto)
			{
				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

				ImGui::PopStyleVar(1);

				ImGuizmo::SetDrawlist();

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

				// menubar
				if (ImGui::BeginMenuBar())
				{
					// debug
					if (ImGui::BeginMenu("debug"))
					{
						ImGui::Separator();
						if (ImGui::RadioButton("enable grid", m_enable_grid)) { m_enable_grid = !m_enable_grid; }
						ImGui::SliderInt("cubes", &m_object_count, 0, 4);
						ImGui::Separator();
						ImGui::EndMenu();
					}
					ImGui::Separator();

					// camera
					if (ImGui::BeginMenu("camera"))
					{
						ImGui::Separator();

						// camera editor
						([](camera & value)
						{
							// clear flags
							auto cf{ value.get_clear_flags() };
							ImGui::TextDisabled("clear flags");
							if (ImGui::CheckboxFlags("color##buffer bit", &cf, gfx::clear_flags_color)) {
								value.set_clear_flags(cf);
							}
							ImGuiExt::Tooltip("color buffer bit");
							ImGui::SameLine();
							if (ImGui::CheckboxFlags("depth##buffer bit", &cf, gfx::clear_flags_depth)) {
								value.set_clear_flags(cf);
							}
							ImGuiExt::Tooltip("depth buffer bit");
							ImGui::SameLine();
							if (ImGui::CheckboxFlags("stencil##buffer bit", &cf, gfx::clear_flags_stencil)) {
								value.set_clear_flags(cf);
							}
							ImGuiExt::Tooltip("stencil buffer bit");

							// background
							if (auto bg{ value.get_background() }
							; ImGui::ColorEdit4("background", bg, ImGuiColorEditFlags_NoInputs)) {
								value.set_background(bg);
							}
							ImGuiExt::Tooltip("clear color");
							ImGui::Separator();

							// projection
							ImGui::TextDisabled("projection");
							bool is_ortho{ value.is_orthographic() };
							if (ImGui::RadioButton("perspective", !is_ortho)) {
								value.set_orthographic(false);
							}
							ImGui::SameLine();
							if (ImGui::RadioButton("orthographic", is_ortho)) {
								value.set_orthographic(true);
							}
							if (auto fov{ value.get_fov() }
							; ImGui::DragFloat("field of view", &fov, .1f, 0.f, 180.f)) {
								value.set_fov(fov);
							}
							if (auto clip{ value.get_clip() }
							; ImGui::DragFloat2("clipping planes", clip)) {
								value.set_clip(clip);
							}
							ImGui::Separator();

							// view
							ImGui::TextDisabled("view");
							if (auto eye{ value.get_eye() }
							; ImGui::DragFloat3("eye", eye, .1f)) {
								value.set_eye(eye);
							}
							if (auto target{ value.get_target() }
							; ImGui::DragFloat3("target", target, .1f)) {
								value.set_target(target);
							}
							if (auto up{ value.get_up() }
							; ImGui::DragFloat3("up", up, .1f)) {
								value.set_up(up);
							}
							if (auto world_up{ value.get_world_up() }
							; ImGui::DragFloat3("world up", world_up, .1f)) {
								value.set_world_up(world_up);
							}
							ImGui::Separator();
						}
						)(m_camera);

						// camera controller editor
						([](camera_controller & value)
						{
							// controller
							ImGui::TextDisabled("controller");
							if (auto pos{ value.get_position() }
							; ImGui::DragFloat3("position", pos, .1f)) {
								value.set_position(pos);
							}
							if (auto yaw{ value.get_yaw() }
							; ImGui::DragFloat("yaw", &yaw, .1f, 0.f, 360.f)) {
								value.set_yaw(yaw);
							}
							if (auto pitch{ value.get_pitch() }
							; ImGui::DragFloat("pitch", &pitch, .1f, 0.f, 360.f)) {
								value.set_pitch(pitch);
							}
							if (auto roll{ value.get_roll() }
							; ImGui::DragFloat("roll", &roll, .1f, 0.f, 360.f)) {
								value.set_roll(roll);
							}
							if (auto zoom{ value.get_zoom() }
							; ImGui::DragFloat("zoom", &zoom, .1f, FLT_MIN, 100.f)) {
								value.set_zoom(zoom);
							}
						});
						ImGui::Separator();

						ImGui::EndMenu();
					}
					ImGui::Separator();

					// transform
					if (ImGui::BeginMenu("transform"))
					{
						ImGui::Separator();
						xedit.ShowOperationControls();
						if (xedit.CurrentGizmoOperation != ImGuizmo::SCALE) {
							xedit.ShowModeControls();
						}
						xedit.ShowSnapControls();
						xedit.ShowBoundsControls();
						ImGui::Separator();
						ImGuiExt::EditTransformMatrix(
							m_object_matrix[m_object_index],
							"position\0rotation\0scale");
						ImGui::Separator();
						ImGui::EndMenu();
					}
					ImGui::Separator();

					// framerate
					ImGui::TextDisabled("%.3f ms/frame ( %.1f fps )", 1000.f / ev->IO.Framerate, ev->IO.Framerate);
					ImGui::Separator();

					ImGui::EndMenuBar();
				}

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

				// viewport
				vec2 const
					win_pos{ (vec2)main_window->get_position() },
					win_size{ (vec2)main_window->get_size() },
					draw_pos{ (vec2)ImGui::GetCursorPos() },
					draw_size{ (vec2)ImGui::GetContentRegionAvail() };
				float_rect const
					bounds{ win_pos + draw_pos, win_size };
				m_view.set_position(draw_pos);
				m_view.set_resolution(draw_size);

				// main image
				ImGui::Image(
					m_view.get_framebuffer()->get_color_attachment(0)->get_handle(),
					draw_size,
					{ 0, 1 }, { 1, 0 },
					colors::white,
					colors::clear);

				// gizmos
				mat4 view{ m_camera.get_view_matrix() }, proj{ m_camera.get_proj_matrix() };
				
				if (m_enable_grid) { ImGuizmo::DrawGrid(view, proj, mat4::identity(), 100.f); }
				
				if (0 < m_object_count) { ImGuizmo::DrawCubes(view, proj, &m_object_matrix[0][0], m_object_count); }
				
				ImGuizmo::SetRect(bounds[0], bounds[1], bounds[2], bounds[3]);

				for (int32 i = 0; i < m_object_count; ++i)
				{
					ImGuizmo::SetID(i);

					bool used{};
					used |= xedit.Manipulate(view, proj, m_object_matrix[i]);
					used |= ImGuizmo::IsUsing();
					if (used) { m_object_index = i; }
				}

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}))
			{
				ImGui::PopStyleVar(1);
			}
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