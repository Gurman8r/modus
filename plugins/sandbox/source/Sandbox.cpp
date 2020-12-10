/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/engine/Application.hpp>
#include <modus_core/engine/PluginManager.hpp>
#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/scene/Components.hpp>
#include <modus_core/graphics/Mesh.hpp>

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
			imgui_demo_panel,
			browser_panel,
			memory_panel,
			settings_panel,
			terminal_panel,
			viewport_panel,
			MAX_PANEL
		};
		ImGuiExt::Panel m_panels[MAX_PANEL]
		{
			{ "Dear ImGui Demo", false },
			{ "browser", false, ImGuiWindowFlags_MenuBar },
			{ "memory", false, ImGuiWindowFlags_MenuBar },
			{ "settings", false, ImGuiWindowFlags_MenuBar },
			{ "terminal", true, ImGuiWindowFlags_MenuBar },
			{ "viewport", true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar },
		};

		// memory editor
		MemoryEditor m_mem_editor{};

		// terminal
		stream_sniper m_cout{ &std::cout }; // stdout wrapper
		ImGuiExt::Terminal m_term{}; // terminal

		// content
		ds::ref<scene> m_active_scene{}; // active scene
		ds::hashmap<ds::string, ds::ref<mesh>> m_meshes{}; // 

		// rendering
		bool m_cycle_bg{ true }; // cycle background
		camera m_camera{}; // camera
		viewport m_viewport{}; // viewport

		// grid
		bool	m_grid_enabled	{ true }; // 
		mat4	m_grid_matrix	{ mat4::identity() }; // 
		float32 m_grid_size		{ 100.f }; // 

		// cubes
		int32 m_object_count{ 1 }; // 
		int32 m_object_index{}; // 
		mat4 m_object_matrix[4] = // 
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
				window_cursor_pos_event,
				window_key_event,
				window_mouse_event,
				imgui_dockspace_event,
				imgui_menubar_event,
				imgui_render_event
			>();
		}

		void on_event(event const & value) final
		{
			switch (value)
			{
			case app_enter_event		::ID: return on_app_enter((app_enter_event &&)value);
			case app_exit_event			::ID: return on_app_exit((app_exit_event &&)value);
			case app_idle_event			::ID: return on_app_idle((app_idle_event &&)value);
			case window_cursor_pos_event::ID: return on_window_cursor_pos((window_cursor_pos_event &&)value);
			case window_key_event		::ID: return on_window_key((window_key_event &&)value);
			case window_mouse_event		::ID: return on_window_mouse((window_mouse_event &&)value);
			case imgui_dockspace_event	::ID: return on_imgui_dockspace((imgui_dockspace_event &&)value);
			case imgui_menubar_event	::ID: return on_imgui_menubar((imgui_menubar_event &&)value);
			case imgui_render_event		::ID: return on_imgui_render((imgui_render_event &&)value);
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

			// meshes
			m_meshes["sphere8x6"] = make_ref<mesh>(get_app()->path_to("assets/models/sphere8x6.obj"));
			m_meshes["sphere32x24"] = make_ref<mesh>(get_app()->path_to("assets/models/sphere32x24.obj"));

			// scene
			m_active_scene = make_ref<scene>(get_bus());
			get_app()->set_active_scene(m_active_scene);

			// viewport
			m_viewport.set_resolution({ 1280, 720 });
			m_viewport.set_framebuffer(gfx::make_framebuffer(m_viewport.get_resolution()));

			// camera
			m_camera.set_clear_flags(gfx::clear_flags_color | gfx::clear_flags_depth);
			m_camera.set_background({ 0.223f, 0.f, 0.46f, 1.f });
			m_camera.set_orthographic(false);
			m_camera.set_eye({ 5.f, 3.f, 5.f });
			m_camera.set_target({ 0.f, 0.f, 0.f });
		}

		void on_app_exit(app_exit_event const & ev)
		{
			debug::success("goodbye!");
		}

		void on_app_idle(app_idle_event const & ev)
		{
			ML_defer(&) { m_term.Output.Dump(m_cout.sstr()); };

			m_viewport.recalculate();

			m_camera.recalculate(m_viewport.get_resolution());

			if (m_cycle_bg) m_camera.set_background
			(
				util::rotate_hue(m_camera.get_background(), ev.delta_time * 10)
			);

			gfx::draw_list draw_data{};
			draw_data +=
			{
				gfx::command::bind_framebuffer(m_viewport.get_framebuffer()),
				gfx::command::set_clear_color(m_camera.get_background()),
				gfx::command::clear(m_camera.get_clear_flags()),
				[&](gfx::render_context * ctx) { /* custom rendering */ },
				gfx::command::bind_framebuffer(0),
			};
			get_app()->get_window()->get_render_context()->execute(draw_data);
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

		void on_imgui_dockspace(imgui_dockspace_event const & ev)
		{
			ev->DockWindow(m_panels + viewport_panel, ev->GetID());
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
				ImGuiExt::MenuItem(m_panels + browser_panel);
				ImGuiExt::MenuItem(m_panels + memory_panel);
				ImGuiExt::MenuItem(m_panels + settings_panel);
				ImGuiExt::MenuItem(m_panels + terminal_panel);
				ImGuiExt::MenuItem(m_panels + viewport_panel);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("help")) {
				ImGuiExt::MenuItem(m_panels + imgui_demo_panel);
				ImGui::EndMenu();
			}
		}

		void on_imgui_render(imgui_render_event const & ev)
		{
			// IMGUI DEMO
			if (m_panels[imgui_demo_panel].IsOpen) {
				ImGui::ShowDemoWindow(&m_panels[imgui_demo_panel].IsOpen);
			}

			draw_browser(ev);	// BROWSER
			draw_memory(ev);	// MEMORY
			draw_settings(ev);	// SETTINGS
			draw_terminal(ev);	// TERMINAL
			draw_viewport(ev);	// VIEWPORT
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_browser(imgui_render_event const & ev)
		{
			if (m_panels[browser_panel].IsOpen) {
				ImGui::SetNextWindowPos({ 64, 64 }, ImGuiCond_Once);
				ImGui::SetNextWindowSize({ 320, 340 }, ImGuiCond_Once);
			}
			if (!m_panels[browser_panel].Draw([&](auto)
			{
				if (ImGui::BeginMenuBar())
				{
					ImGui::EndMenuBar();
				}
				ImGui::Separator();
				ImGui::Separator();
			}))
			{
			}
		}

		void draw_memory(imgui_render_event const & ev)
		{
			static size_t selected_index{};

			memory_manager * const mman{ get_global<memory_manager>() };
			passthrough_resource * const mres{ mman->get_resource() };
			memory_manager::record_storage const & mrec{ mman->get_storage() };

			auto highlight = [&](byte * ptr, size_t size)
			{
				auto const addr{ std::distance(mres->begin(), ptr) };
				m_panels[memory_panel].IsOpen = true;
				ImGui::SetWindowFocus(m_panels[memory_panel].Title);
				m_mem_editor.GotoAddrAndHighlight((size_t)addr, (size_t)addr + size);
			};

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
				ImGui::SetNextWindowSize(winsize * 0.5f, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize * 0.5f, ImGuiCond_Once, { 0.5f, 0.5f });
			}

			m_panels[memory_panel].Draw([&](auto)
			{
				// menubar
				if (ImGui::BeginMenuBar())
				{
					// read only
					ImGui::Checkbox("read only", &m_mem_editor.ReadOnly);
					ImGui::Separator();

					// home/end
					if (ImGui::Button("home")) { highlight(mres->begin(), 1); }
					if (ImGui::Button("end")) { highlight(mres->end() - sizeof(void *), 1); }
					ImGui::Separator();

					// mrec
					ImGui::PushItemWidth(256);
					char selected_label[32] = "";
					std::sprintf(selected_label, "%p", mman->get_record_addr(selected_index));
					if (ImGui::BeginCombo("##mrec", selected_label, ImGuiComboFlags_None))
					{
						static auto const initial_width{ ImGui::GetContentRegionAvailWidth() };
						ImGui::Columns(4);

						static ML_block(w = initial_width) { ImGui::SetColumnWidth(-1, w * 0.2f); };
						ImGui::Text("index"); ImGui::NextColumn();

						static ML_block(w = initial_width) { ImGui::SetColumnWidth(-1, w * 0.4f); };
						ImGui::Text("address"); ImGui::NextColumn();

						static ML_block(w = initial_width) { ImGui::SetColumnWidth(-1, w * 0.2f); };
						ImGui::Text("size"); ImGui::NextColumn();

						static ML_block(w = initial_width) { ImGui::SetColumnWidth(-1, w * 0.2f); };
						ImGui::Text("count"); ImGui::NextColumn();

						ImGui::Separator();
						for (size_t i = 0; i < mrec.size(); ++i)
						{
							mrec.expand_all(i, [&](auto addr, auto index, auto count, auto size)
							{
								ImGuiExt_ScopeID(addr);
								ImGui::TextDisabled("%u", index); ImGui::NextColumn();
								char buf[20] = ""; std::sprintf(buf, "%p", addr);
								bool const pressed{ ImGui::Selectable(buf) }; ImGui::NextColumn();
								ImGui::TextDisabled("%u", size); ImGui::NextColumn();
								ImGui::TextDisabled("%u", count); ImGui::NextColumn();
								if (pressed)
								{
									selected_index = i;
									highlight(addr, size);
								}
							});
						}

						ImGui::Columns(1);
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
					ImGui::Separator();

					// progress
					char progress[32] = ""; std::sprintf(progress,
						"%u / %u (%.2f%%)",
						(uint32_t)mres->used(),
						(uint32_t)mres->capacity(),
						mres->percentage()
					);
					ImGui::ProgressBar(mres->fraction(), { 256.f, 0.f }, progress);
					ImGuiExt::TooltipEx([&]() noexcept
					{
						ImGui::Text("allocations: %u", mres->count());
						ImGui::Text("total:       %u", mres->capacity());
						ImGui::Text("in use:      %u", mres->used());
						ImGui::Text("available:   %u", mres->free());
					});
					ImGui::Separator();

					ImGui::EndMenuBar();
				}

				// contents
				m_mem_editor.DrawContents(mres->data(), mres->capacity(), mres->base());
			});
		}

		void draw_settings(imgui_render_event const & ev)
		{
			auto const & dev{ get_app()->get_window()->get_render_device() };
			auto const & inf{ dev->get_info() };
			auto const & ctx{ dev->get_active_context() };

			if (m_panels[settings_panel].IsOpen) {
				ImGui::SetNextWindowPos({ 64, 64 }, ImGuiCond_Once);
				ImGui::SetNextWindowSize({ 320, 340 }, ImGuiCond_Once);
			}
			
			if (!m_panels[settings_panel].Draw([&](auto)
			{
				if (ImGui::BeginMenuBar())
				{
					ImGui::Text(inf.vendor.c_str()); ImGuiExt::Tooltip("vendor"); ImGui::Separator();
					ImGui::Text(inf.renderer.c_str()); ImGuiExt::Tooltip("renderer"); ImGui::Separator();
					ImGui::Text(inf.version.c_str()); ImGuiExt::Tooltip("version"); ImGui::Separator();
					ImGui::Text(inf.shading_language_version.c_str()); ImGuiExt::Tooltip("shading language version"); ImGui::Separator();
					ImGui::EndMenuBar();
				}
				ImGui::Separator();

				if (gfx::alpha_state astate; ImGui::CollapsingHeader("alpha") && ctx->get_alpha_state(&astate))
				{
					ImGui::Checkbox("enabled", &astate.enabled);
					ImGui::Text("pred: %s (%u)", gfx::predicate_NAMES[astate.pred], astate.pred);
					ImGui::Text("ref: %f", astate.ref);
				}
				ImGui::Separator();

				if (gfx::blend_state bstate; ImGui::CollapsingHeader("blend") && ctx->get_blend_state(&bstate))
				{
					ImGui::Checkbox("enabled", &bstate.enabled);
					ImGui::ColorEdit4("color", bstate.color);
					ImGui::Text("color equation: %s (%u)", gfx::equation_NAMES[bstate.color_equation], bstate.color_equation);
					ImGui::Text("color sfactor: %s (%u)", gfx::factor_NAMES[bstate.color_sfactor], bstate.color_sfactor);
					ImGui::Text("color dfactor: %s (%u)", gfx::factor_NAMES[bstate.color_dfactor], bstate.color_dfactor);
					ImGui::Text("alpha equation: %s (%u)", gfx::equation_NAMES[bstate.alpha_equation], bstate.alpha_equation);
					ImGui::Text("alpha sfactor: %s (%u)", gfx::factor_NAMES[bstate.alpha_sfactor], bstate.alpha_sfactor);
					ImGui::Text("alpha dfactor: %s (%u)", gfx::factor_NAMES[bstate.alpha_dfactor], bstate.alpha_dfactor);
				}
				ImGui::Separator();

				if (gfx::cull_state cstate; ImGui::CollapsingHeader("cull") && ctx->get_cull_state(&cstate))
				{
					ImGui::Checkbox("enabled", &cstate.enabled);
					ImGui::Text("facet: %s (%u)", gfx::facet_NAMES[cstate.facet], cstate.facet);
					ImGui::Text("order: %s (%u)", gfx::order_NAMES[cstate.order], cstate.order);
				}
				ImGui::Separator();

				if (gfx::depth_state dstate; ImGui::CollapsingHeader("depth") && ctx->get_depth_state(&dstate))
				{
					ImGui::Checkbox("enabled", &dstate.enabled);
					ImGui::Text("pred: %s (%u) ", gfx::predicate_NAMES[dstate.pred], dstate.pred);
					ImGui::Text("range: %f, %f", dstate.range[0], dstate.range[1]);
				}
				ImGui::Separator();

				if (gfx::stencil_state sstate; ImGui::CollapsingHeader("stencil") && ctx->get_stencil_state(&sstate))
				{
					ImGui::Checkbox("enabled", &sstate.enabled);
					ImGui::Text("front pred: %s (%u)", gfx::predicate_NAMES[sstate.front.pred], sstate.front.pred);
					ImGui::Text("front ref: %i", sstate.front.ref);
					ImGui::Text("front mask: %u", sstate.front.mask);
					ImGui::Text("back pred: %s (%u)", gfx::predicate_NAMES[sstate.back.pred], sstate.back.pred);
					ImGui::Text("back ref: %i", sstate.back.ref);
					ImGui::Text("back mask: %u", sstate.back.mask);
				}
				ImGui::Separator();
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
					// execute
					get_app()->get_interpreter()->exec(line);
				});
			};

			if (m_panels[terminal_panel].IsOpen) {
				static auto const winsize{ (vec2)get_app()->get_window()->get_size() };
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
			
			if (!m_panels[terminal_panel].Draw([&](auto)
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
			static ImGuiExt::Overlay debug_overlay{ "debug_overlay", true, -1, { 32, 32 }, .35f };

			static ImGuiExt::TransformEditor xedit{};

			ImGuizmo::SetOrthographic(m_camera.is_orthographic());

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
			if (!m_panels[viewport_panel].Draw([&](auto)
			{
				ImGui::PopStyleVar(1);

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

				// menubar
				if (ImGui::BeginMenuBar())
				{
					// debug
					if (ImGui::BeginMenu("debug"))
					{
						ImGui::Separator();

						ImGui::TextDisabled("overlay");
						ImGui::SameLine();
						ImGui::Checkbox("enabled##overlay", &debug_overlay.IsOpen);
						ImGui::Separator();

						ImGui::TextDisabled("grid");
						ImGui::SameLine();
						if (ImGui::RadioButton("enabled##grid", m_grid_enabled)) { m_grid_enabled = !m_grid_enabled; }
						ImGui::SameLine();
						ImGui::DragFloat("##grid size", &m_grid_size, .1f, 1.f, 1000.f, "size: %.1f");
						ImGui::Separator();

						ImGui::TextDisabled("cubes");
						ImGui::SameLine();
						ImGui::SliderInt("##cube count", &m_object_count, 0, 4);
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
							ImGuiExt_ScopeID(&value);

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
							ImGui::Separator();
						}
						)(m_camera);

						// camera controller editor
						([](camera_controller & value)
						{
							ImGuiExt_ScopeID(&value);

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
							ImGui::Separator();
						});

						ImGui::EndMenu();
					}
					ImGui::Separator();

					// transform
					if (ImGui::BeginMenu("transform"))
					{
						ImGui::Separator();
						xedit.ShowOperationControls();
						xedit.ShowModeControls();
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

					ImGui::EndMenuBar();
				}

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

				// main image
				m_viewport.set_bounds({ ev->Viewports[0]->GetWorkPos(), ev->Viewports[0]->GetWorkSize() });
				ImGui::Image(
					m_viewport.get_framebuffer()->get_color_attachment(0)->get_handle(),
					m_viewport.get_resolution(),
					{ 0, 1 }, { 1, 0 },
					colors::white,
					colors::clear);

				// overlay
				debug_overlay.Draw([&
					, fps	= get_app()->get_fps()
					, input	= get_app()->get_input()
				](ImGuiExt::Overlay * o)
				{
					ImGui::Text("%.3f ms/frame ( %.1f fps )", 1000.f / fps->value, fps->value);
					ImGui::Separator();
					
					if (!ImGui::IsMousePosValid()) { ImGui::Text("cursor pos: <invalid>"); }
					else { ImGui::Text("cursor pos: (%.1f,%.1f)", input->cursor_pos[0], input->cursor_pos[1]); }
					
					if (ImGui::BeginPopupContextWindow()) {
						if (ImGui::MenuItem("custom", 0, o->Corner == -1)) { o->Corner = -1; }
						if (ImGui::MenuItem("top-left", 0, o->Corner == 0)) { o->Corner = 0; }
						if (ImGui::MenuItem("top-right", 0, o->Corner == 1)) { o->Corner = 1; }
						if (ImGui::MenuItem("bottom-left", 0, o->Corner == 2)) { o->Corner = 2; }
						if (ImGui::MenuItem("bottom-right", 0, o->Corner == 3)) { o->Corner = 3; }
						if (ImGui::MenuItem("close")) { o->IsOpen = false; }
						ImGui::EndPopup();
					}
				});
				
				// gizmos
				mat4 const
					view_matrix{ m_camera.get_view_matrix() },
					proj_matrix{ m_camera.get_proj_matrix() };
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect
				(
					m_viewport.get_left(),
					m_viewport.get_top(),
					m_viewport.get_width(),
					m_viewport.get_height()
				);
				if (m_grid_enabled) {
					ImGuizmo::DrawGrid(view_matrix, proj_matrix, m_grid_matrix, m_grid_size);
				}
				if (0 < m_object_count) {
					ImGuizmo::DrawCubes(view_matrix, proj_matrix, &m_object_matrix[0][0], m_object_count);
				}
				for (int32 i = 0; i < m_object_count; ++i) {
					ImGuizmo::SetID(i);
					if (xedit.Manipulate(view_matrix, proj_matrix, m_object_matrix[i])) {
						m_object_index = i;
					}
				}

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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