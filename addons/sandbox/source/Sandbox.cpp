#include "./Sandbox.hpp"

namespace ml
{
	struct fps_camera : behavior_script
	{
		using behavior_script::behavior_script;

		void on_create() override {}

		void on_destroy() override {}

		void on_update(duration dt) override {}
	};

	struct ML_PLUGIN_API sandbox final : addon
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool // imgui panels
				m_show_imgui_demo			{ false },
				m_show_imgui_metrics		{ false },
				m_show_imgui_style_editor	{ false },
				m_show_imgui_about			{ false };

		bool // main panels
				m_show_viewport				{ true },
				m_show_terminal				{ false },
				m_show_scene_editor			{ true };
		
		// debug overlay
		bool	m_show_overlay				{ true };
		int32	m_overlay_corner			{ 1 };
		vec2	m_overlay_offset			{ 32, 32 };
		float32	m_overlay_alpha				{ 0.35f };
		
		stream_sniper m_cout{ &std::cout }; // 
		ImGuiExt::Terminal m_terminal{}; // 
		ImGuiExt::TransformEditor m_xeditor{}; // 

		bool	m_grid_enabled{ true }; // 
		mat4	m_grid_matrix{ mat4::identity() }; // 
		float32 m_grid_size{ 100.f }; // 

		// scenes
		scene_editor m_scene_editor{};
		hash_map<string, ref<scene_tree>> m_scenes{};

		// resources
		list<ref<gfx::framebuffer>> m_framebuffers{}; // framebuffers
		hash_map<string, ref<gfx::texture>> m_textures{}; // textures
		hash_map<string, ref<gfx::program>> m_programs{}; // programs
		hash_map<string, ref<gfx::shader>> m_shaders{}; // shaders
		hash_map<string, ref<mesh>> m_meshes{}; // meshes

		// rendering
		bool m_shift_bg_hue{ true }; // cycle background
		viewport m_viewport{}; // viewport
		camera m_camera{}; // camera
		camera_controller m_cc{ &m_camera }; // camera controller
		bool m_dragging_view{}; // dragging view

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

		sandbox(addon_manager * manager, void * userptr) : addon{ manager, userptr }
		{
			subscribe<
				runtime_startup_event,
				runtime_shutdown_event,
				runtime_idle_event,
				dockspace_builder_event,
				runtime_gui_event,
				runtime_end_frame_event,

				char_event,
				key_event,
				mouse_button_event,
				mouse_pos_event,
				mouse_wheel_event
			>();
		}

		void on_event(event const & value) final
		{
			switch (value)
			{
			case runtime_startup_event	::ID: return on_runtime_startup((runtime_startup_event const &)value);
			case runtime_shutdown_event	::ID: return on_runtime_shutdown((runtime_shutdown_event const &)value);
			case runtime_idle_event		::ID: return on_runtime_update((runtime_idle_event const &)value);
			case dockspace_builder_event::ID: return on_dockspace_builder((dockspace_builder_event const &)value);
			case runtime_gui_event		::ID: return on_runtime_gui((runtime_gui_event const &)value);
			case runtime_end_frame_event::ID: return on_runtime_frame_end((runtime_end_frame_event const &)value);

			case char_event				::ID: return on_char((char_event const &)value);
			case key_event				::ID: return on_key((key_event const &)value);
			case mouse_button_event		::ID: return on_mouse_button((mouse_button_event const &)value);
			case mouse_pos_event		::ID: return on_mouse_pos((mouse_pos_event const &)value);
			case mouse_wheel_event		::ID: return on_mouse_wheel((mouse_wheel_event const &)value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_runtime_startup(runtime_startup_event const & ev)
		{
			// path to
			auto path2 = std::bind(&core_application::get_path_to, ev.ptr, std::placeholders::_1);

			// icons
			if (bitmap const i{ path2("resource/modus_launcher.png"), false })
			{
				ev->get_main_window()->set_icons(1, i.width(), i.height(), i.data());
			}

			// framebuffers
			m_framebuffers.push_back(gfx::framebuffer::create({ 1280, 720 }));

			// textures
			m_textures["earth_dm_2k"] = gfx::texture2d::create(path2("assets/textures/earth/earth_dm_2k.png"), gfx::texture_flags_default);
			m_textures["earth_sm_2k"] = gfx::texture2d::create(path2("assets/textures/earth/earth_sm_2k.png"), gfx::texture_flags_default);

			// shaders
			if (gfx::program_source src{}
			; gfx::parse_source(path2("addons/sandbox/resource/shaders/basic_3D.shader"), src))
			{
				if (src[0]) { m_shaders["vs"] = gfx::shader::create({ 0, { *src[0] } }); }
				if (src[1]) { m_shaders["ps"] = gfx::shader::create({ 1, { *src[1] } }); }
			}
			
			// programs
			m_programs["2D"] = gfx::parse_program(path2("addons/sandbox/resource/shaders/basic_2D.shader"));
			m_programs["3D"] = gfx::parse_program(path2("addons/sandbox/resource/shaders/basic_3D.shader"));

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
			auto & tree = m_scenes["0"] = make_ref<scene_tree>("New Scene");
			m_scene_editor.set_context(tree);
			if (auto n{ tree->get_root()->new_child("New Entity") })
			{
				auto & ent = n->emplace<entity>(tree->create_entity());
				auto & tag = ent.add_component<tag_component>(n->get_name());
				auto & xfm = ent.add_component<transform_component>();
				auto & cam = ent.add_component<camera_component>();
				auto & scr = ent.add_component<behavior_component>();
			}
			
			// terminal
			m_terminal.UserName = "root";
			m_terminal.HostName = "localhost";
			m_terminal.PathName = "~";
			m_terminal.ModeName = "";
			m_terminal.Commands.push_back({ "clear", {}, [&](auto line) { m_terminal.Output.Lines.clear(); } });
			m_terminal.Commands.push_back({ "exit", {}, [&](auto line) { ML_get_global(application)->quit(); } });
			m_terminal.Commands.push_back({ "help", {}, [&](auto line) { for (auto const & e : m_terminal.Commands) { debug::puts("/{0}", e.name); } } });
			m_terminal.Commands.push_back({ "history", {}, [&](auto line) { for (auto const & e : m_terminal.History) { debug::puts(e); } } });
			m_terminal.Commands.push_back({ "python", {}, [&](auto line) {
				if (m_terminal.ModeName.empty() && line.empty()) {
					m_terminal.ModeName = "python"; return; // lock
				}
				else if (m_terminal.ModeName == line && line == "python") {
					m_terminal.ModeName.clear(); return; // unlock
				}
				py::exec((std::string)line); // execute
			} });
		}

		void on_runtime_shutdown(runtime_shutdown_event const & ev)
		{
			debug::good("goodbye!");
		}

		void on_runtime_update(runtime_idle_event const & ev)
		{
			string const str{ m_cout.str() };
			m_terminal.Output.Print(str);
			m_cout.str({});

			duration const dt{ ev->get_delta_time() };

			input_state * const input{ ev->get_input() };

			vec2 const view_size{ m_viewport.get_rect().size() };

			for (auto const & fb : m_framebuffers) { fb->resize(view_size); }

			if (m_shift_bg_hue) { m_camera.set_background(util::rotate_hue(m_camera.get_background(), dt * 10)); }

			if (m_dragging_view) {
				vec2 const md{ input->mouse_delta * (dt * 50) };
				m_cc.yaw(-md[0]);
				m_cc.pitch(+md[1]);
				m_dragging_view = false;
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
		
		void on_dockspace_builder(dockspace_builder_event const & ev)
		{
			ImGuiID root{ ev->ID };
			ImGuiID left{ ev->SplitNode(root, ImGuiDir_Left, 0.25f, nullptr, &root) };
			ev->DockWindow("viewport", root);
			ev->DockWindow("scene editor", left);
		}

		void on_runtime_gui(runtime_gui_event const & ev)
		{
			application * const		app				{ ML_get_global(application) };
			native_window * const	window			{ app->get_main_window() };
			vec2 const				winsize			{ (vec2)window->get_size() };
			ImGuiContext * const	imgui			{ app->get_imgui().get() };
			ImGuiStyle &			style			{ imgui->Style };
			float_rect const &		view_rect		{ m_viewport.get_rect() };
			vec2 const				work_pos		{ view_rect.position() };
			vec2 const				work_size		{ view_rect.size() };
			float32 const			dt				{ app->get_delta_time() };
			float32 const			fps				{ app->get_fps()->value };
			float32 const			time			{ app->get_time().count() };
			input_state * const		input			{ app->get_input() };
			vec2 const &			mouse_pos		{ input->mouse_pos };
			vec2 const &			mouse_delta		{ input->mouse_delta };
			float32 const &			mouse_wheel		{ input->mouse_wheel };
			mat4 const &			view_matrix		{ m_camera.get_view_matrix() };
			mat4 const &			proj_matrix		{ m_camera.get_proj_matrix() };
			
			// GIZMOS
			ImGuizmo::SetOrthographic(m_camera.is_orthographic());

			// MAIN MENU BAR
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("file")) {
					if (ImGui::MenuItem("new", "ctrl+n")) {}
					if (ImGui::MenuItem("open", "ctrl+o")) {}
					ImGui::Separator();
					if (ImGui::MenuItem("close")) {}
					ImGui::Separator();
					if (ImGui::MenuItem("save", "ctrl+s")) {}
					if (ImGui::MenuItem("save as", "ctrl+shift+s")) {}
					ImGui::Separator();
					if (ImGui::MenuItem("exit", "alt+f4")) { app->quit(); }
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
					if (ImGui::MenuItem("overlay", "", &m_show_overlay)) {}
					if (ImGui::MenuItem("terminal", "", &m_show_terminal)) {}
					if (ImGui::MenuItem("viewport", "", &m_show_viewport)) {}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("help")) {
					if (ImGui::MenuItem("Dear ImGui Demo", "", &m_show_imgui_demo)) {}
					if (ImGui::MenuItem("Dear ImGui Metrics", "", &m_show_imgui_metrics)) {}
					if (ImGui::MenuItem("Dear ImGui Style Editor", "", &m_show_imgui_style_editor)) {}
					if (ImGui::MenuItem("About Dear ImGui", "", &m_show_imgui_about)) {}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			// IMGUI DEMO
			if (m_show_imgui_demo) { ImGui::ShowDemoWindow(&m_show_imgui_demo); }
			if (m_show_imgui_metrics) { ImGui::ShowMetricsWindow(&m_show_imgui_metrics); }
			if (m_show_imgui_style_editor) { ImGui::Begin("Dear ImGui Style Editor", &m_show_imgui_style_editor); ImGui::ShowStyleEditor(&imgui->Style); ImGui::End(); }
			if (m_show_imgui_about) { ImGui::ShowAboutWindow(&m_show_imgui_about); }

			// VIEWPORT
			if (m_show_viewport)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
				bool const is_open{ ImGui::Begin(
					"viewport",
					&m_show_viewport,
					ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar
				) };
				ImGui::PopStyleVar(1);
				if (is_open)
				{
					// menubar
					if (ImGui::BeginMenuBar()) {
						if (ImGui::BeginMenu("debug")) {
							ImGui::TextDisabled("overlay"); ImGui::SameLine();
							if (ImGui::RadioButton("##toggle_overlay", m_show_overlay)) { m_show_overlay = !m_show_overlay; }
							ImGui::TextDisabled("grid"); ImGui::SameLine();
							if (ImGui::RadioButton("##grid enabled", m_grid_enabled)) { m_grid_enabled = !m_grid_enabled; } ImGui::SameLine();
							ImGui::DragFloat("##grid size", &m_grid_size, .1f, 1.f, 1000.f, "size: %.1f");
							ImGui::TextDisabled("cubes"); ImGui::SameLine();
							ImGui::SliderInt("##cube count", &m_object_count, 0, 4);
							ImGui::EndMenu();
						}
						ImGui::Separator();
						if (ImGui::BeginMenu("camera")) {
							edit_camera(m_camera);
							edit_camera_controller(m_cc);
							ImGui::EndMenu();
						}
						ImGui::Separator();
						if (ImGui::BeginMenu("transform")) {
							ImGui::Separator();
							m_xeditor.ShowOperationControls();
							m_xeditor.ShowModeControls();
							m_xeditor.ShowSnapControls();
							m_xeditor.ShowBoundsControls();
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
					
					// main image
					ImRect const bb{ imgui->CurrentWindow->InnerRect };
					m_viewport.set_rect((vec4)bb.ToVec4());
					ImGui::ItemSize(bb);
					if (ImGui::ItemAdd(bb, NULL)) {
						imgui->CurrentWindow->DrawList->AddImage(
							m_framebuffers[0]->get_color_attachments()[0]->get_handle(),
							bb.Min, bb.Max, { 0, 1 }, { 1, 0 }, 0xffffffff);
						m_dragging_view =
							!ImGuizmo::IsUsing() &&
							ImGui::IsItemHovered() &&
							ImGui::IsMouseDragging(0);
					}

					// gizmos
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
						if (m_xeditor.Manipulate(view_matrix, proj_matrix, m_object_matrix[i])) {
							m_object_index = i;
						}
					}
				}
				ImGui::End();
			}

			// TERMINAL
			if (m_show_terminal)
			{
				ImGui::SetNextWindowSize(winsize / 2, ImGuiCond_Once);
				ImGui::SetNextWindowPos(winsize / 2, ImGuiCond_Once, { 0.5f, 0.5f });
				m_terminal.Draw("terminal", &m_show_terminal, ImGuiWindowFlags_MenuBar);
			}

			// OVERLAY
			if (m_show_overlay)
			{
				bool const no_move{ m_overlay_corner != -1 };
				if (no_move)
				{
					auto const vp{ imgui->Viewports[0] };
					float_rect const bounds{
						vp->GetWorkPos(),
						vp->GetWorkSize()
					};
					vec2 const pos{
						((m_overlay_corner & 1) ? (bounds[0] + bounds[2] - m_overlay_offset[0]) : (bounds[0] + m_overlay_offset[0])),
						((m_overlay_corner & 2) ? (bounds[1] + bounds[3] - m_overlay_offset[1]) : (bounds[1] + m_overlay_offset[1]))
					};
					vec2 const piv{
						((m_overlay_corner & 1) ? 1.f : 0.f),
						((m_overlay_corner & 2) ? 1.f : 0.f)
					};
					ImGui::SetNextWindowPos(pos, ImGuiCond_Always, piv);
					ImGui::SetNextWindowViewport(vp->ID);
				}
				ImGui::SetNextWindowBgAlpha(m_overlay_alpha);
				bool const is_open{ ImGui::Begin(
					"overlay",
					&m_show_overlay,
					ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_NoDocking |
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoFocusOnAppearing |
					ImGuiWindowFlags_NoNav |
					(no_move ? ImGuiWindowFlags_NoMove : 0)
				) };
				if (is_open)
				{
					ImGui::TextDisabled("debug");
					ImGui::Text("%.3f ms/frame ( %.1f fps )", 1000.f / fps, fps);
					ImGui::Text("time: %.2f", time);
					ImGui::Text("view rect: (%.1f,%.1f,%.1f,%.1f)", view_rect[0], view_rect[1], view_rect[2], view_rect[3]);
					if (ImGui::IsItemHovered()) {
						ImGui::GetForegroundDrawList()->AddRect(view_rect.min(), view_rect.max(), IM_COL32(255, 155, 0, 255));
					}
					ImGui::NewLine();

					ImGui::TextDisabled("input");
					if (!ImGui::IsMousePosValid((ImVec2 *)&mouse_pos)) { ImGui::Text("mouse pos: <invalid>"); }
					else { ImGui::Text("mouse pos: (%.1f,%.1f)", mouse_pos[0], mouse_pos[1]); }
					ImGui::Text("mouse delta: (%.1f,%.1f)", mouse_delta[0], mouse_delta[1]);
					ImGui::Text("mouse wheel: %.1f", mouse_wheel);
					ImGui::Text("mouse: ");
					for (size_t i = 0; i < mouse_button_MAX; ++i) {
						if (input->mouse_down[i]) {
							ImGui::SameLine();
							ImGui::Text("(b%i:%.2fs)", i, input->mouse_down_duration[i]);
						}
					}
					ImGui::Text("keys: ");
					for (size_t i = 0; i < keycode_MAX; ++i) {
						if (input->keys_down[i]) {
							ImGui::SameLine();
							ImGui::Text("(%i:%.2fs)", i, input->keys_down_duration[i]);
						}
					}
					ImGui::Text("mods: %s%s%s%s",
						input->is_shift ? "shift " : "",
						input->is_ctrl ? "ctrl " : "",
						input->is_alt ? "alt " : "",
						input->is_super ? "super " : "");

					if (ImGui::BeginPopupContextWindow()) {
						if (ImGui::MenuItem("custom", 0, m_overlay_corner == -1)) { m_overlay_corner = -1; }
						if (ImGui::MenuItem("top-left", 0, m_overlay_corner == 0)) { m_overlay_corner = 0; }
						if (ImGui::MenuItem("top-right", 0, m_overlay_corner == 1)) { m_overlay_corner = 1; }
						if (ImGui::MenuItem("bottom-left", 0, m_overlay_corner == 2)) { m_overlay_corner = 2; }
						if (ImGui::MenuItem("bottom-right", 0, m_overlay_corner == 3)) { m_overlay_corner = 3; }
						if (ImGui::MenuItem("close")) { m_show_overlay = false; }
						ImGui::EndPopup();
					}
				}
				ImGui::End();
			}

			// scene editor
			if (ImGui::Begin("scene editor", NULL, ImGuiWindowFlags_MenuBar))
			{
				static auto const & scene0{ m_scenes["0"] };

				if (ImGui::BeginMenuBar()) {
					ImGui::TextDisabled("scene editor");
					ImGui::EndMenuBar();
				}
				
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2, 2 });
				m_scene_editor.draw_hierarchy(
					ImGuiTreeNodeFlags_DefaultOpen |
					ImGuiTreeNodeFlags_Framed |
					ImGuiTreeNodeFlags_FramePadding |
					ImGuiTreeNodeFlags_SpanAvailWidth);
				ImGui::PopStyleVar();
			}
			ImGui::End();
		}

		void on_runtime_frame_end(runtime_end_frame_event const & ev)
		{
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
	ML_PLUGIN_API ml::addon * ml_create_addon(ml::addon_manager * manager, void * userptr)
	{
		return manager->allocate<ml::sandbox>(userptr);
	}

	ML_PLUGIN_API void ml_destroy_addon(ml::addon_manager * manager, ml::addon * ptr)
	{
		manager->deallocate<ml::sandbox>(ptr);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */