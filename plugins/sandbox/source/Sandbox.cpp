#include "./Sandbox.hpp"

namespace ml
{
	struct ML_PLUGIN_API sandbox final : plugin
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// panels
		enum
		{
			imgui_demo_panel,
			imgui_metrics_panel,
			imgui_style_panel,
			imgui_about_panel,

			memory_panel,
			terminal_panel,
			viewport_panel,

			MAX_PANEL
		};
		ImGuiExt::Panel m_panels[MAX_PANEL]
		{
			{ "Dear ImGui Demo", false },
			{ "Dear ImGui Metrics", false },
			{ "Dear ImGui Style Editor", false },
			{ "About Dear ImGui", false },

			{ "memory", false, ImGuiWindowFlags_MenuBar },
			{ "terminal", false, ImGuiWindowFlags_MenuBar },
			{ "viewport", true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar },
		};

		ImGuiExt::Overlay m_overlay{ "overlay", true, -1, { 32, 32 }, .35f };

		// terminal
		stream_sniper m_cout{ &std::cout }; // stdout wrapper
		ImGuiExt::Terminal m_term{}; // terminal

		// resources
		ds::list<ref<gfx::framebuffer>> m_framebuffers{}; // framebuffers
		ds::hashmap<ds::string, ref<gfx::texture>> m_textures{}; // textures
		ds::hashmap<ds::string, ref<gfx::program>> m_programs{}; // programs
		ds::hashmap<ds::string, ref<gfx::shader>> m_shaders{}; // shaders
		ds::hashmap<ds::string, ref<mesh>> m_meshes{}; // meshes
		ds::hashmap<ds::string, ref<scene>> m_scenes{}; // scenes

		// rendering
		bool m_shift_bg_hue{ true }; // cycle background
		viewport m_viewport{}; // viewport
		camera m_camera{}; // camera
		camera_controller m_cc{ &m_camera }; // camera controller
		bool m_drag_view{}; // dragging view

		// grid
		bool	m_grid_enabled	{ true }; // 
		mat4	m_grid_matrix	{ mat4::identity() }; // 
		float32 m_grid_size		{ 100.f }; // 

		// cubes
		int32 m_object_count{ 1 }; // 
		int32 m_object_index{ 0 }; // 
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
				load_event, unload_event,

				begin_frame_event, end_frame_event,

				dock_builder_event, main_menu_bar_event, imgui_event,

				char_event, key_event, mouse_button_event, mouse_pos_event, mouse_wheel_event
			>();
		}

		void on_event(event const & value) final
		{
			switch (value)
			{
			case load_event			::ID: return on_load((load_event const &)value);
			case unload_event		::ID: return on_unload((unload_event const &)value);
			
			case begin_frame_event	::ID: return on_begin_frame((begin_frame_event const &)value);
			case end_frame_event	::ID: return on_end_frame((end_frame_event const &)value);
			
			case dock_builder_event	::ID: return on_dock_builder((dock_builder_event const &)value);
			case main_menu_bar_event::ID: return on_main_menu_bar((main_menu_bar_event const &)value);
			case imgui_event		::ID: return on_imgui((imgui_event const &)value);

			case char_event			::ID: return on_char((char_event const &)value);
			case key_event			::ID: return on_key((key_event const &)value);
			case mouse_button_event	::ID: return on_mouse_button((mouse_button_event const &)value);
			case mouse_pos_event	::ID: return on_mouse_pos((mouse_pos_event const &)value);
			case mouse_wheel_event	::ID: return on_mouse_wheel((mouse_wheel_event const &)value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_load(load_event const & ev)
		{
			// path to
			auto path2 = std::bind(&core_application::path_to, ev.ptr, std::placeholders::_1);

			// icons
			if (bitmap const i{ path2("resource/modus_launcher.png"), false })
			{
				ev->get_window()->set_icons(1, i.width(), i.height(), i.data());
			}

			// framebuffers
			m_framebuffers.push_back(gfx::framebuffer::create({ 1280, 720 }));

			// textures
			m_textures["earth_dm_2k"] = gfx::texture2d::create(path2("assets/textures/earth/earth_dm_2k.png"), gfx::texture_flags_default);
			m_textures["earth_sm_2k"] = gfx::texture2d::create(path2("assets/textures/earth/earth_sm_2k.png"), gfx::texture_flags_default);

			// shaders
			if (gfx::program_source src{}
			; shader_parser::parse(path2("plugins/sandbox/resource/shaders/basic_3D.shader"), src))
			{
				if (src[0]) { m_shaders["vs"] = gfx::shader::create({ 0, { *src[0] } }); }
				if (src[1]) { m_shaders["ps"] = gfx::shader::create({ 1, { *src[1] } }); }
			}
			
			// programs
			m_programs["2D"] = shader_parser::make_program(path2("plugins/sandbox/resource/shaders/basic_2D.shader"));
			m_programs["3D"] = shader_parser::make_program(path2("plugins/sandbox/resource/shaders/basic_3D.shader"));

			// meshes
			m_meshes["sphere8x6"] = make_ref<mesh>(path2("assets/models/sphere8x6.obj"));
			m_meshes["sphere32x24"] = make_ref<mesh>(path2("assets/models/sphere32x24.obj"));

			// viewport
			m_viewport.set_rect({ 0, 0, 1280, 720 });

			// camera
			m_camera.set_clear_flags(gfx::clear_flags_color | gfx::clear_flags_depth);
			m_camera.set_background({ 0.223f, 0.f, 0.46f, 1.f });
			m_camera.set_orthographic(false);
			m_camera.set_eye({ -5.f, 3.f, -5.f });
			m_cc.set_position(m_camera.get_eye());
			m_cc.set_yaw(45.f);
			m_cc.set_pitch(-25.f);

			// scene
			auto & scene0 = m_scenes["0"] = make_ref<scene>();
			ev->set_active_scene(scene0);
			entity e{ scene0->new_entity() };
		}

		void on_unload(unload_event const & ev)
		{
			ev->set_active_scene(nullptr);

			debug::success("goodbye!");
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_begin_frame(begin_frame_event const & ev)
		{
			ds::string const str{ m_cout.str() };
			m_term.Output.Print(str);
			m_cout.str({});

			duration const dt{ ev->delta_time() };

			input_state * const input{ ev->get_input() };

			vec2 const view_size{ m_viewport.get_rect().size() };

			for (auto const & fb : m_framebuffers) { fb->resize(view_size); }

			if (m_shift_bg_hue) { m_camera.set_background(util::rotate_hue(m_camera.get_background(), dt * 10)); }

			if (m_drag_view) {
				vec2 const md{ input->mouse_delta * (dt * 50) };
				m_cc.yaw(-md[0]);
				m_cc.pitch(+md[1]);
				m_drag_view = false;
			}
			vec3 pos{ m_cc.get_position() };
			if (input->keys_down[keycode_w]) {}
			if (input->keys_down[keycode_a]) {}
			if (input->keys_down[keycode_s]) {}
			if (input->keys_down[keycode_d]) {}
			m_cc.set_position(pos);
			m_cc.recalculate(view_size);

			static auto const & pgm{ m_programs["3D"] };
			static auto const & tex{ m_textures["earth_dm_2k"] };
			static auto const & msh{ m_meshes["sphere32x24"] };
			static auto const & vs{ m_shaders["vs"] }, & ps{ m_shaders["ps"] };
			ev->get_render_context()->execute
			(
				gfx::command::bind_framebuffer(m_framebuffers[0]),
				gfx::command::set_clear_color(m_camera.get_background()),
				gfx::command::clear(m_camera.get_clear_flags()),
				[&](gfx::render_context * ctx)
				{
					if (0 == m_object_count) { return; }
					pgm->bind();
					pgm->set_uniform("u_model", m_object_matrix[0]);
					pgm->set_uniform("u_view", m_camera.get_view_matrix());
					pgm->set_uniform("u_proj", m_camera.get_proj_matrix());
					pgm->set_uniform("u_color", (vec4)colors::white);
					pgm->set_uniform("u_texture", tex);
					pgm->bind_textures();
					ctx->draw(msh->get_vertexarray());
					pgm->unbind();
				},
				gfx::command::bind_framebuffer(0)
			);
		}

		void on_end_frame(end_frame_event const & ev)
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_dock_builder(dock_builder_event const & ev)
		{
			ev->DockWindow(m_panels + viewport_panel, ev->GetID());
		}

		void on_main_menu_bar(main_menu_bar_event const & ev)
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
				if (ImGui::MenuItem("exit", "alt+f4")) { ML_get_global(application)->quit(); }
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
				ImGuiExt::MenuItem(m_panels + terminal_panel);
				ImGuiExt::MenuItem(m_panels + viewport_panel);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("help")) {
				ImGuiExt::MenuItem(m_panels + imgui_demo_panel);
				ImGuiExt::MenuItem(m_panels + imgui_metrics_panel);
				ImGuiExt::MenuItem(m_panels + imgui_style_panel);
				ImGuiExt::MenuItem(m_panels + imgui_about_panel);
				ImGui::EndMenu();
			}
		}

		void on_imgui(imgui_event const & ev)
		{
			if (m_panels[imgui_demo_panel].IsOpen) { ImGui::ShowDemoWindow(&m_panels[imgui_demo_panel].IsOpen); }
			if (m_panels[imgui_metrics_panel].IsOpen) { ImGui::ShowMetricsWindow(&m_panels[imgui_metrics_panel].IsOpen); }
			if (m_panels[imgui_style_panel].IsOpen) { ImGui::ShowStyleEditor(&ev->Style); }
			if (m_panels[imgui_about_panel].IsOpen) { ImGui::ShowAboutWindow(&m_panels[imgui_about_panel].IsOpen); }

			draw_memory(ev);	// MEMORY
			draw_terminal(ev);	// TERMINAL
			draw_viewport(ev);	// VIEWPORT
			draw_overlay(ev);	// OVERLAY
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_memory(imgui_event const & ev)
		{
			memory_manager * const mman{ ML_get_global(memory_manager) };
			memory_manager::record_storage const & mrec{ mman->get_storage() };
			passthrough_resource * const mres{ mman->get_resource() };
			byte * const
				buffer_data{ mres->buffer_data() };
			size_t const
				num_allocations{ mres->num_allocations() },
				buffer_size{ mres->buffer_size() },
				buffer_used{ mres->buffer_used() },
				buffer_free{ mres->buffer_free() };
			float32 const
				fraction_used{ (float32)buffer_used / (float32)buffer_size },
				percent_used{ fraction_used * 100.f };

			static size_t selected_index{};
			static MemoryEditor medit{};
			static ML_block(&) // setup
			{
				medit.Open				= true;
				medit.ReadOnly			= true;
				medit.Cols				= 32;
				medit.OptShowOptions	= true;
				medit.OptShowDataPreview= true;
				medit.OptShowHexII		= false;
				medit.OptShowAscii		= true;
				medit.OptGreyOutZeroes	= true;
				medit.OptUpperCaseHex	= true;
				medit.OptMidColsCount	= 8;
				medit.OptAddrDigitsCount= 0;
				medit.HighlightColor	= IM_COL32(0, 255, 255, 50);
				medit.ReadFn			= nullptr;
				medit.WriteFn			= nullptr;
				medit.HighlightFn		= nullptr;
			};

			auto highlight = [&](byte * ptr, size_t size)
			{
				auto const addr{ std::distance(mres->begin(), ptr) };
				m_panels[memory_panel].IsOpen = true;
				ImGui::SetWindowFocus(m_panels[memory_panel].Title);
				medit.GotoAddrAndHighlight((size_t)addr, (size_t)addr + size);
			};

			if (m_panels[memory_panel].IsOpen) {
				
				static vec2 const winsize{ (vec2)ev->IO.DisplaySize };
				ImGui::SetNextWindowSize(winsize * 0.5f, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize * 0.5f, ImGuiCond_Once, { 0.5f, 0.5f });
			}
			m_panels[memory_panel].Draw([&](auto)
			{
				// MENUBAR
				if (ImGui::BeginMenuBar())
				{
					// READ-ONLY
					ImGui::Checkbox("read only", &medit.ReadOnly);
					ImGui::Separator();

					// HOME / END
					if (ImGui::Button("home")) { highlight(mres->begin(), 1); }
					if (ImGui::Button("end")) { highlight(mres->end() - sizeof(void *), 1); }
					ImGui::Separator();

					// RECORDS
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

					// PROGRESS BAR
					char progress_str[32]{};
					std::sprintf(progress_str, "%zu / %zu (%.2f%%)",
						buffer_used,
						buffer_size,
						fraction_used * 100.f);
					ImGui::ProgressBar(fraction_used, { 256.f, 0.f }, progress_str);

					ImGuiExt::TooltipEx([&]() noexcept
					{
						ImGui::Text("allocations: %zu", num_allocations);
						ImGui::Text("total:       %zu", buffer_size);
						ImGui::Text("in use:      %zu", buffer_used);
						ImGui::Text("available:   %zu", buffer_free);
					});
					ImGui::Separator();

					ImGui::EndMenuBar();
				}

				// contents
				medit.DrawContents(buffer_data, buffer_size, (size_t)buffer_data);
			});
		}

		void draw_overlay(imgui_event const & ev)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 1, 1 });
			ML_defer(&) { ImGui::PopStyleVar(1); };
			m_overlay.Draw(ev->Viewports[0], [&](ImGuiExt::Overlay * o) noexcept
			{
				ImGui::TextDisabled("debug");
				{
					float32 const fps{ ev->IO.Framerate };
					ImGui::Text("%.3f ms/frame ( %.1f fps )", 1000.f / fps, fps);

					float32 const time{ ML_get_global(application)->time().count() };
					ImGui::Text("time: %.2f", time);

					float_rect const & view_rect{ m_viewport.get_rect() };
					ImGui::Text("view rect: (%.1f,%.1f,%.1f,%.1f)", view_rect[0], view_rect[1], view_rect[2], view_rect[3]);
					if (ImGui::IsItemHovered()) {
						ImGui::GetForegroundDrawList()->AddRect(view_rect.min(), view_rect.max(), IM_COL32(255, 155, 0, 255));
					}
				}
				ImGui::NewLine();
				ImGui::TextDisabled("input");
				{
					vec2 const mouse_pos{ (vec2)ev->IO.MousePos };
					if (!ImGui::IsMousePosValid((ImVec2 *)&mouse_pos)) { ImGui::Text("mouse pos: <invalid>"); }
					else { ImGui::Text("mouse pos: (%.1f,%.1f)", mouse_pos[0], mouse_pos[1]); }

					vec2 const mouse_delta{ (vec2)ev->IO.MouseDelta };
					ImGui::Text("mouse delta: (%.1f,%.1f)", mouse_delta[0], mouse_delta[1]);

					float32 const mouse_wheel{ ev->IO.MouseWheel };
					ImGui::Text("mouse wheel: %.1f", mouse_wheel);

					ImGui::Text("mouse: ");
					for (size_t i = 0; i < mouse_button_MAX; ++i) {
						if (ev->IO.MouseDown[i]) {
							ImGui::SameLine();
							ImGui::Text("(b%i:%.2fs)", i, ev->IO.MouseDownDuration[i]);
						}
					}

					ImGui::Text("keys: ");
					for (size_t i = 0; i < keycode_MAX; ++i) {
						if (ev->IO.KeysDown[i]) {
							ImGui::SameLine();
							ImGui::Text("(%i:%.2fs)", i, ev->IO.KeysDownDuration[i]);
						}
					}

					ImGui::Text("mods: %s%s%s%s",
						ev->IO.KeyShift	? "shift "	: "",
						ev->IO.KeyCtrl	? "ctrl "	: "",
						ev->IO.KeyAlt	? "alt "	: "",
						ev->IO.KeySuper	? "super "	: "");
				}

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
		}

		void draw_terminal(imgui_event const & ev)
		{
			static ML_block(&) // setup
			{
				// session
				m_term.UserName = "root";
				m_term.HostName = "localhost";
				m_term.PathName = "~";
				m_term.ModeName = "";

				// clear
				m_term.AddCommand("clear", {}, [&](auto line) {
					m_term.Output.Lines.clear();
				});

				// exit
				m_term.AddCommand("exit", {}, [&](auto line) {
					ML_get_global(application)->quit();
				});

				// help
				m_term.AddCommand("help", {}, [&](auto line) {
					for (auto const & name : m_term.CommandData.get<0>()) {
						debug::puts("/{0}", name);
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
					if (m_term.ModeName.empty() && line.empty()) {
						m_term.ModeName = "python"; return;
					}
					// unlock
					else if (m_term.ModeName == line && line == "python") {
						m_term.ModeName.clear(); return;
					}
					// execute
					py::exec((std::string)line);
				});
			};

			if (m_panels[terminal_panel].IsOpen) {
				
				static vec2 const winsize{ (vec2)ev->IO.DisplaySize };
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

		void draw_viewport(imgui_event const & ev)
		{
			static ImGuiExt::TransformEditor xedit{};
			
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

						ImGui::TextDisabled("grid");
						ImGui::SameLine();
						if (ImGui::RadioButton("##grid enabled", m_grid_enabled)) { m_grid_enabled = !m_grid_enabled; }
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
							; ImGuiExt::EditVec3("eye", eye, .1f, 0.f, 0.f, "%.1f", 0.f, 56.f)) {
								value.set_eye(eye);
							}
							if (auto target{ value.get_target() }
							; ImGuiExt::EditVec3("target", target, .1f, 0.f, 0.f, "%.1f", 0.f, 56.f)) {
								value.set_target(target);
							}
							if (auto up{ value.get_up() }
							; ImGuiExt::EditVec3("up", up, .1f, 0.f, 0.f, "%.1f", 0.f, 56.f)) {
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
							; ImGui::DragFloat3("position", pos, .1f, 0.f, 0.f, "%.1f")) {
								value.set_position(pos);
							}
							if (auto yaw{ value.get_yaw() }
							; ImGui::DragFloat("yaw", &yaw, .1f, 0.f, 0.f, "%.1f")) {
								value.set_yaw(yaw);
							}
							if (auto pitch{ value.get_pitch() }
							; ImGui::DragFloat("pitch", &pitch, .1f, 0.f, 0.f, "%.1f")) {
								value.set_pitch(pitch);
							}
							if (auto roll{ value.get_roll() }
							; ImGui::DragFloat("roll", &roll, .1f, 0.f, 0.f, "%.1f")) {
								value.set_roll(roll);
							}
							if (auto zoom{ value.get_zoom() }
							; ImGui::DragFloat("zoom", &zoom, .1f, FLT_MIN, 100.f, "%.1f")) {
								value.set_zoom(zoom);
							}
							ImGui::Separator();
						}
						)(m_cc);

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
							"pos\0rot\0scl", 0.01f, 0.f, 0.f, "%.01f", 0.f, 36.f);
						ImGui::Separator();
						ImGui::EndMenu();
					}
					ImGui::Separator();

					ImGui::EndMenuBar();
				}

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

				// main image
				ImRect const bb{ ev->CurrentWindow->InnerRect };
				m_viewport.set_rect((vec4)bb.ToVec4());
				Widgets::Image(m_framebuffers[0]->get_color_attachments()[0], bb);
				m_drag_view = ImGui::IsItemHovered() && ImGui::IsMouseDragging(0);

				// gizmos
				mat4 const
					view_matrix{ m_camera.get_view_matrix() },
					proj_matrix{ m_camera.get_proj_matrix() };
				ImGuizmo::SetOrthographic(m_camera.is_orthographic());
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(bb.Min.x, bb.Min.y, bb.GetWidth(), bb.GetHeight());
				if (m_grid_enabled) {
					ImGuizmo::DrawGrid(view_matrix, proj_matrix, m_grid_matrix, m_grid_size);
				}
				if (0 && 0 < m_object_count) {
					ImGuizmo::DrawCubes(view_matrix, proj_matrix, &m_object_matrix[0][0], m_object_count);
				}
				for (int32 i = 0; i < m_object_count; ++i) {
					ImGuizmo::SetID(i);
					if (xedit.Manipulate(view_matrix, proj_matrix, m_object_matrix[i])) {
						m_object_index = i;
						m_drag_view = false;
					}
				}

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}))
			{
				ImGui::PopStyleVar(1);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_char(char_event const & ev)
		{
		}

		void on_key(key_event const & ev)
		{
		}

		void on_mouse_button(mouse_button_event const & ev)
		{
		}

		void on_mouse_pos(mouse_pos_event const & ev)
		{
		}

		void on_mouse_wheel(mouse_wheel_event const & ev)
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