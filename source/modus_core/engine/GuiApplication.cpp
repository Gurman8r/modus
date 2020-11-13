#include <modus_core/engine/GuiApplication.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32_t argc, char * argv[], allocator_type alloc)
		: core_application	{ argc, argv, alloc }
		, m_window			{ get_bus(), alloc }
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

		auto const mainloop{ get_main_loop() };
		mainloop->set_loop_condition(&main_window::is_open, get_main_window());
		mainloop->set_enter_callback([&]() { get_bus()->fire<app_enter_event>(); });
		mainloop->set_exit_callback([&]() { get_bus()->fire<app_exit_event>(); });
		mainloop->set_idle_callback([&]() { get_bus()->fire<app_idle_event>(); });
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
						_ML ImGui_LoadStyle(path_to(style_prefs));
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

			// poll events
			get_window_context()->poll_events();

			// imgui layer
			get_main_window()->do_imgui_frame([&
				, imgui		= get_imgui().get()
				, menubar	= get_menubar()
				, dockspace	= get_dockspace()
			]()
			{
				dockspace->SetWindowFlag(
					ImGuiWindowFlags_MenuBar,
					ImGui::FindWindowByName(menubar->Title));
				
				(*dockspace)(imgui->Viewports[0], [&]() noexcept
				{
					if (ImGuiID const id{ dockspace->GetID() }; !ImGui::DockBuilderGetNode(id))
					{
						ImGui::DockBuilderRemoveNode(id);
						ImGui::DockBuilderAddNode(id, dockspace->DockFlags);
						get_bus()->fire<imgui_dockspace_event>(dockspace);
						ImGui::DockBuilderFinish(id);
					}
				});

				get_bus()->fire<imgui_render_event>(imgui);
			});

			// swap buffers
			if (get_main_window()->has_hints(window_hints_doublebuffer))
			{
				get_window_context()->swap_buffers(get_main_window()->get_handle());
			}

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