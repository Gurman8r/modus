#include <modus_core/engine/GuiApplication.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32_t argc, char * argv[], allocator_type alloc)
		: core_application	{ argc, argv, alloc }
		, m_window			{ new main_window{ get_bus(), alloc } }
		, m_loop_timer		{ false }
		, m_delta_time		{}
		, m_frame_count		{}
		, m_frame_rate		{}
		, m_fps_accum		{}
		, m_fps_index		{}
		, m_fps_times		{}
		, m_cursor_pos		{}
		, m_mouse			{}
		, m_keyboard		{}
	{
		ML_assert(begin_singleton<gui_application>(this));
		
		subscribe<window_cursor_pos_event, window_key_event, window_mouse_event>();
	}

	gui_application::~gui_application() noexcept
	{
		ML_assert(end_singleton<gui_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_event(event const & value)
	{
		core_application::on_event(value);
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };

			// setup window
			if (attr().contains("window")) {
				json & window_prefs{ attr("window") };
				ML_assert(get_main_window()->open(
					window_prefs["title"],
					window_prefs["video"],
					window_prefs["context"],
					window_prefs["hints"]
				));
			}

			// setup imgui
			if (attr().contains("imgui")) {
				json & gui_prefs{ attr("imgui") };
				if (gui_prefs.contains("menubar")) {
					get_menubar()->Configure(gui_prefs["menubar"]);
				}
				if (gui_prefs.contains("dockspace")) {
					get_dockspace()->Configure(gui_prefs["dockspace"]);
				}
				if (gui_prefs.contains("style")) {
					json & style_prefs{ gui_prefs["style"] };
					if (style_prefs.is_string()) {
						ImGui_LoadStyle(path_to(style_prefs));
					}
				}
			}
		} break;

		case app_exit_event::ID: {
			auto && ev{ (app_exit_event &&)value };
		} break;

		case app_idle_event::ID: {
			auto && ev{ (app_idle_event &&)value };

			// benchmarks
			++m_frame_count;
			m_delta_time = m_loop_timer.elapsed();
			m_loop_timer.restart();
			auto const dt{ (float_t)m_delta_time.count() };
			m_fps_accum += dt - m_fps_times[m_fps_index];
			m_fps_times[m_fps_index] = dt;
			m_fps_index = (m_fps_index + 1) % m_fps_times.size();
			m_frame_rate = (0.f < m_fps_accum)
				? 1.f / (m_fps_accum / (float_t)m_fps_times.size())
				: FLT_MAX;

			// poll
			get_window_manager()->poll_events();

			// imgui
			get_main_window()->do_imgui_frame([&
				, imgui = get_imgui_context()
				, docker = get_dockspace()
				, menubar = get_menubar()
			]() noexcept
			{
				docker->SetWindowFlag(ImGuiWindowFlags_MenuBar, ImGuiExt::FindWindowByName(menubar));
				
				(*docker)(imgui->Viewports[0], [&]() noexcept
				{
					if (ImGuiID const id{ docker->GetID() }; !ImGui::DockBuilderGetNode(id))
					{
						ImGui::DockBuilderRemoveNode(id);
						ImGui::DockBuilderAddNode(id, docker->DockFlags);
						get_bus()->fire<imgui_dockspace_event>(docker);
						ImGui::DockBuilderFinish(id);
					}
				});

				get_bus()->fire<imgui_render_event>(imgui);
			});

		} break;

		case window_key_event::ID: {
			auto && ev{ (window_key_event &&)value };
			m_keyboard[ev.key] = ev.action;
		} break;

		case window_mouse_event::ID: {
			auto && ev{ (window_mouse_event &&)value };
			m_mouse[ev.button] = ev.action;
		} break;

		case window_cursor_pos_event::ID: {
			auto && ev{ (window_cursor_pos_event &&)value };
			m_cursor_pos = { ev.x, ev.y };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global gui_application
namespace ml::globals
{
	static gui_application * g_gui_app{};

	ML_impl_global(gui_application) get() noexcept
	{
		return g_gui_app;
	}

	ML_impl_global(gui_application) set(gui_application * value) noexcept
	{
		return g_gui_app = value;
	}
}